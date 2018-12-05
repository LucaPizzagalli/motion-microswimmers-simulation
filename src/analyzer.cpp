#include "analyzer.hpp"
#include <fstream>
#include <gsl/gsl_integration.h>
#include <cmath>

#include "bacterium.hpp"

#define MAP_X 1000
#define MAP_Y 1000

Analyzer::Analyzer(double left_x, double top_y, double right_x, double bottom_y)
{
    this->probability_map = std::vector<std::vector<double>>(MAP_X, std::vector<double>(MAP_Y, 0));
    this->n_map_points = 0;

    this->probability_map_left_corner_x = left_x;
    this->probability_map_top_corner_y = top_y;
    this->probability_map_right_corner_x = right_x;
    this->probability_map_bottom_corner_y = bottom_y;
    this->size_cell_x = (this->probability_map_right_corner_x - this->probability_map_left_corner_x) / MAP_X;
    this->size_cell_y = (this->probability_map_bottom_corner_y - this->probability_map_top_corner_y) / MAP_Y;
}

void Analyzer::update_probability_map(Simulation *world, int start_time_step, int end_time_step, int step_size)
{
    for (int time = start_time_step; time < end_time_step; time+=step_size)
    {
        double x = world->get_bacterium()->get_history_body_x(time);
        double y = world->get_bacterium()->get_history_body_x(time);
        if (x > this->probability_map_left_corner_x && x < this->probability_map_right_corner_x && y > this->probability_map_top_corner_y && y < this->probability_map_bottom_corner_y)
            this->probability_map[(int)((x - this->probability_map_left_corner_x) / size_cell_x)][(int)((y - this->probability_map_top_corner_y) / size_cell_y)]++;
    }
    this->n_map_points += (end_time_step - start_time_step)/step_size;
}

void Analyzer::compute_radial_probability(double radius, double center_x, double center_y)
{
    double delta_r = 5.;
    int n_points = (int)(radius / std::max(this->size_cell_x, this->size_cell_y))/2;
    double d_r = radius / n_points;
    int n_local_points = (int)(delta_r / d_r + 0.5);
    std::vector <double> local_p = std::vector<double>(n_points, 0);
    this->radial_probability_r = std::vector<double>(n_points - n_local_points + 1, 0);
    this->radial_probability_p = std::vector<double>(n_points - n_local_points + 1, 0);

    for (int i = 0; i < n_points - n_local_points + 1; i++)
        this->radial_probability_r[i] = (n_local_points - 1 + i) * d_r;

    for (int x = 0; x < MAP_X; x++)
        for (int y = 0; y < MAP_Y; y++)
        {
            double distance = sqrt((x * this->size_cell_x + this->probability_map_left_corner_x - center_x) * (x * this->size_cell_x + this->probability_map_left_corner_x - center_x) + (y * this->size_cell_y + this->probability_map_top_corner_y - center_y) * (y * this->size_cell_y + this->probability_map_top_corner_y - center_y));
            int index = (int)(distance / d_r);
            if (index < n_points)
                local_p[index] += this->probability_map[x][y];
        }

    for(int i = 0; i < n_points - n_local_points + 1; i++)
        for(int j = 0; j < n_local_points; j++)
            this->radial_probability_p[i] += local_p[i + j];

    double integral = 0;
    for(int i = 0; i < n_points - n_local_points + 1; i++)
    {
        this->radial_probability_p[i] /= (2 * M_PI * this->radial_probability_r[i] * delta_r);
        integral += this->radial_probability_p[i];
    }
    integral *= d_r;

    for(int i = 0; i < n_points - n_local_points + 1; i++)
        this->radial_probability_p[i] /= integral;
}

double Analyzer::compute_near_wall_probability(double radius)
{
    double near_wall = 15; //15 micrometer
    double dr = this->radial_probability_r[1] - this->radial_probability_r[0];
    double probability = 0;
    for (unsigned int i = 0; i < this->radial_probability_r.size(); i++)
        if (this->radial_probability_r[i] < radius - near_wall)
            probability += this->radial_probability_p[i] * dr;
    return 1 - radius / (radius - near_wall) * probability;
}

void Analyzer::save_probability_map(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (auto &row : this->probability_map)
    {
        for (std::vector<double>::size_type i=0; i< row.size()-1; i++)
            out << row[i] / this->n_map_points << ",";
        out << row[row.size()-1] / this->n_map_points;
        out << "\n";
    }
    out.close();
}

void Analyzer::save_radial_probability(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (unsigned int i=0; i< this->radial_probability_r.size(); i++)
        out << this->radial_probability_r[i] << "," << this->radial_probability_p[i] << "\n";
    out.close();
}