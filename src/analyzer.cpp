#include "analyzer.hpp"
#include <sstream>
#include <fstream>
#include <gsl/gsl_integration.h>

#include "cell.hpp"

Analyzer::Analyzer(nlohmann::json simulation_parameters, nlohmann::json physics_parameters)
{
    this->map_stats = simulation_parameters["compute_probability_map"].get<bool>();
    this->end_map_stats = simulation_parameters["compute_end_probability_map"].get<bool>();
    this->time_step_size = simulation_parameters["time_step"].get<double>();
    if (this->map_stats || this->end_map_stats)
    {
        this->map_width = simulation_parameters["probability_map_width"].get<int>();
        this->map_height = simulation_parameters["probability_map_height"].get<int>();
        this->probability_map = std::vector<std::vector<double>>(map_width, std::vector<double>(map_height, 0));
        this->n_map_points = 0;
    }
    if (this->map_stats)
    {
        this->wall_radius = physics_parameters["wallDisk"]["innerRadius"].get<double>();
        this->probability_map_left_corner_x = -this->wall_radius;
        this->probability_map_top_corner_y = -this->wall_radius;
        this->probability_map_right_corner_x = this->wall_radius;
        this->probability_map_bottom_corner_y = this->wall_radius;
        this->size_cell_x = (this->probability_map_right_corner_x - this->probability_map_left_corner_x) / map_width;
        this->size_cell_y = (this->probability_map_bottom_corner_y - this->probability_map_top_corner_y) / map_height;
    }
    if (this->end_map_stats)
    {
        this->probability_map_top_corner_y = physics_parameters["wallTop"]["y"].get<double>();
        this->probability_map_bottom_corner_y = physics_parameters["wallBottom"]["y"].get<double>();
        this->probability_map_left_corner_x = physics_parameters["wallLeft"]["x"].get<double>();
        this->probability_map_right_corner_x = physics_parameters["wallRight"]["x"].get<double>();
        this->size_cell_x = (this->probability_map_right_corner_x - this->probability_map_left_corner_x) / map_width;
        this->size_cell_y = (this->probability_map_bottom_corner_y - this->probability_map_top_corner_y) / map_height;
    }

    this->displacement_stats = simulation_parameters["compute_displacement"].get<bool>();
    if (this->displacement_stats)
    {
        int memory_size = simulation_parameters["n_saved_time_steps"].get<int>();
        this->displacement = std::vector<double>(memory_size, 0);
        this->n_tracks = 0;
    }

    this->diffusion_stats = simulation_parameters["compute_diffusion"].get<bool>();
    if (this->diffusion_stats)
    {
        this->probability_map_top_corner_y = physics_parameters["wallTop"]["y"].get<double>();
        this->probability_map_bottom_corner_y = physics_parameters["wallBottom"]["y"].get<double>();
        this->memory_size = simulation_parameters["n_saved_time_steps"].get<int>();
        this->probability_map_height = simulation_parameters["probability_map_height"].get<int>();
        this->size_cell_y = (physics_parameters["wallBottom"]["y"].get<double>() - physics_parameters["wallTop"]["y"].get<double>()) / this->probability_map_height;
        this->gradient = std::vector<double>((this->memory_size) * (this->probability_map_height - 1), 0);
        this->flux = std::vector<double>((this->memory_size) * (this->probability_map_height - 1), 0);
    }

    this->save_trajectories = simulation_parameters["save_trajectory"].get<bool>();
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();
}

void Analyzer::update_stats(Simulation *world, int start_time_step, int end_time_step, int step_size)
{
    std::vector<Cell> cell = world->get_cells();
    for (unsigned int i = 0; i < cell.size(); i++)
    {
        if (this->map_stats)
        {
            for (int time = start_time_step; time < end_time_step; time += step_size)
            {
                Vector2D coord = cell[i].get_instance(time).coord;
                if (coord[0] > this->probability_map_left_corner_x && coord[0] < this->probability_map_right_corner_x && coord[1] > this->probability_map_top_corner_y && coord[1] < this->probability_map_bottom_corner_y)
                    this->probability_map[(int)((coord[0] - this->probability_map_left_corner_x) / size_cell_x)][(int)((coord[1] - this->probability_map_top_corner_y) / size_cell_y)]++;
            }
            this->n_map_points += (end_time_step - start_time_step) / step_size;
        }
        else if (this->end_map_stats)
        {
            float time = end_time_step - step_size;
            Vector2D coord = cell[i].get_instance(time).coord;
            if (coord[0] > this->probability_map_left_corner_x && coord[0] < this->probability_map_right_corner_x && coord[1] > this->probability_map_top_corner_y && coord[1] < this->probability_map_bottom_corner_y)
                this->probability_map[(int)((coord[0] - this->probability_map_left_corner_x) / size_cell_x)][(int)((coord[1] - this->probability_map_top_corner_y) / size_cell_y)]++;
            this->n_map_points = 1;
        }
        if (this->displacement_stats)
        {
            for (int time = start_time_step; time < end_time_step; time += step_size)
            {
                Vector2D coord = cell[i].get_instance(time).coord;
                this->displacement[time / step_size] += coord * coord;
            }
            this->n_tracks++;
        }
        if (this->save_trajectories)
        {
            std::stringstream strm;
            strm << "output/" << i << "_trajectory.csv";
            this->save_trajectory(strm.str().c_str(), &(cell[i]), start_time_step, end_time_step);
        }
    }
    if (this->diffusion_stats)
    {
        std::vector<int> prev_density_probability = std::vector<int>(this->probability_map_height, 0);
        std::vector<int> next_density_probability = std::vector<int>(this->probability_map_height, 0);
        int int_time = 0;
        for (int time = start_time_step; time < end_time_step; time += step_size)
        {
            prev_density_probability = next_density_probability;
            std::fill(next_density_probability.begin(), next_density_probability.end(), 0);
            for (unsigned int i = 0; i < cell.size(); i++)
            {
                double y = cell[i].get_instance(time).coord[1];
                next_density_probability[(int)((y - this->probability_map_top_corner_y) / this->size_cell_y)] += 1;
            }
            if (int_time > 0)
            {
                for (int i = 1; i < this->probability_map_height; i++)
                {
                    this->gradient[(int_time - 1) * (this->probability_map_height - 1) + i - 1] = next_density_probability[i] - next_density_probability[i - 1];
                    this->flux[(int_time - 1) * (this->probability_map_height - 1) + i - 1] = next_density_probability[i] - prev_density_probability[i];
                }
            }
            int_time++;
        }
        printf("%d", 55);
    }
}

void Analyzer::compute_stats()
{
    if (this->map_stats)
    {
        this->compute_radial_probability(0., 0.);
        this->compute_near_wall_probability();
    }
    if (this->displacement_stats)
        this->compute_displacement();
}

void Analyzer::compute_displacement()
{
    for (unsigned int i = 0; i < this->displacement.size(); i++)
        this->displacement[i] /= n_tracks;
}

void Analyzer::compute_radial_probability(double center_x, double center_y)
{
    int n_points = (int)(this->wall_radius / std::max(this->size_cell_x, this->size_cell_y));
    double d_r = this->wall_radius / n_points;
    double delta_r = 5 * d_r;
    int n_local_points = (int)(delta_r / d_r + 0.5);
    std::vector<double> local_p = std::vector<double>(n_points, 0);
    this->radial_probability_r = std::vector<double>(n_points - n_local_points + 1, 0);
    this->radial_probability_p = std::vector<double>(n_points - n_local_points + 1, 0);

    for (int i = 0; i < n_points - n_local_points + 1; i++)
        this->radial_probability_r[i] = (n_local_points + i) * d_r;

    for (int x = 0; x < this->map_width; x++)
        for (int y = 0; y < this->map_height; y++)
        {
            double distance = sqrt((x * this->size_cell_x + this->probability_map_left_corner_x - center_x) * (x * this->size_cell_x + this->probability_map_left_corner_x - center_x) + (y * this->size_cell_y + this->probability_map_top_corner_y - center_y) * (y * this->size_cell_y + this->probability_map_top_corner_y - center_y));
            int index = (int)(distance / d_r);
            if (index < n_points)
                local_p[index] += this->probability_map[x][y];
        }

    for (int i = 0; i < n_points - n_local_points + 1; i++)
        for (int j = 0; j < n_local_points; j++)
            this->radial_probability_p[i] += local_p[i + j];

    double integral = 0;
    for (int i = 0; i < n_points - n_local_points + 1; i++)
    {
        this->radial_probability_p[i] /= (2 * M_PI * this->radial_probability_r[i] * delta_r);
        integral += this->radial_probability_p[i];
    }
    integral *= d_r;

    for (int i = 0; i < n_points - n_local_points + 1; i++)
        this->radial_probability_p[i] /= integral;
}

void Analyzer::compute_near_wall_probability()
{
    double near_wall = 15; //15 micrometer
    double dr = this->radial_probability_r[1] - this->radial_probability_r[0];
    double probability = 0;
    for (unsigned int i = 0; i < this->radial_probability_r.size(); i++)
        if (this->radial_probability_r[i] < this->wall_radius - near_wall)
            probability += this->radial_probability_p[i] * dr;
    this->near_wall_probability = 1 - this->wall_radius / (this->wall_radius - near_wall) * probability;
}

void Analyzer::save_stats(const std::string &file_name)
{
    if (this->end_map_stats)
    {
        std::stringstream strm;
        strm << file_name << "_probability_map.csv";
        this->save_probability_map(strm.str().c_str());
    }
    if (this->map_stats)
    {
        std::stringstream strm;
        strm << file_name << "_probability_map.csv";
        this->save_probability_map(strm.str().c_str());
        strm.str("");
        strm << file_name << "_radial_probability.csv";
        this->save_radial_probability(strm.str().c_str());
        strm.str("");
        strm << file_name << "_near_wall_probability.csv";
        this->save_near_wall_probability(strm.str().c_str());
    }
    if (this->displacement_stats)
    {
        std::stringstream strm;
        strm << file_name << "_displacement.csv";
        this->save_displacement(strm.str().c_str());
    }
    if (this->diffusion_stats)
    {
        std::stringstream strm;
        strm << file_name << "_diffusion.csv";
        this->save_diffusion(strm.str().c_str());
    }
}

void Analyzer::save_probability_map(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (auto &row : this->probability_map)
    {
        for (unsigned int i = 0; i < row.size(); i++) /// row.size()-1
            out << row[i] / this->n_map_points << ",";
        out << row[row.size()] / this->n_map_points;
        out << "\n";
    }
    out.close();
}

void Analyzer::save_radial_probability(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (unsigned int i = 0; i < this->radial_probability_r.size(); i++)
        out << this->radial_probability_r[i] << "," << this->radial_probability_p[i] << "\n";
    out.close();
}

void Analyzer::save_near_wall_probability(const std::string &file_name)
{
    std::ofstream out(file_name);
    out << this->wall_radius << "," << this->near_wall_probability;
    out.close();
}

void Analyzer::save_displacement(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (unsigned int i = 0; i < this->displacement.size(); i++)
        out << this->time_step_size * (this->step_size * (i + 1) - 1) << "," << this->displacement[i] << "\n";
    out.close();
}

void Analyzer::save_trajectory(const std::string &file_name, Cell *cell, int start_time_step, int end_time_step)
{
    std::ofstream out(file_name);
    for (int i = start_time_step; i < end_time_step; i += this->step_size)
        out << this->time_step_size * (i + this->step_size - 1) << "," << cell->get_instance(i).coord[0] << "," << cell->get_instance(i).coord[1] << "\n";
    out.close();
}

void Analyzer::save_diffusion(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (int i = 500; i < (this->memory_size - 1) * (this->probability_map_height - 1); i++)
        out << this->gradient[i] << "," << this->flux[i] << "\n";
}