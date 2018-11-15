#include "analyzer.h"
#include <fstream>
#include <gsl/gsl_integration.h>

#include "bacterium.h"

#define MAP_X 1000
#define MAP_Y 1000
#define delta_r 5

Analyzer::Analyzer()
{
    this->reset_probability_map();
}

void Analyzer::reset_probability_map()
{
    this->probability_map = std::vector<std::vector<double>>(MAP_X, std::vector<double>(MAP_Y, 0));
    this->n_map_points = 0;
}

void Analyzer::compute_probability_map(Simulation *world, int start_time_step, int end_time_step, double left_x, double top_y, double right_x, double bottom_y)
{
    this->size_cell_x = (right_x - left_x) / MAP_X;
    this->size_cell_y = (bottom_y - top_y) / MAP_Y;

    for (int time = start_time_step; time < end_time_step; time++)
    {
        double x = world->get_bacterium()->get_body_x(time);
        double y = world->get_bacterium()->get_body_y(time);
        if (x > left_x && x < right_x && y > top_y && y < bottom_y)
            this->probability_map[(int)((x - left_x) / size_cell_x)][(int)((y - top_y) / size_cell_y)]++;
    }
    this->n_map_points += end_time_step - start_time_step;
}

double Analyzer::count_occurrences(double radius, double center_x, double center_y)
{
    int occurrences = 0;
    for (int x = 0; x < MAP_X; x++)
        for (int y = 0; y < MAP_Y; y++)
        {
            double distance = (x * this->size_cell_x - center_x) * (x * this->size_cell_x - center_x) + (y * this->size_cell_y - center_y) * (y * this->size_cell_y - center_y);
            if (distance >= (radius - delta_r) * (radius - delta_r) && distance < radius * radius)
                occurrences += this->probability_map[x][y];
        }
    return occurrences / (2 * M_PI * radius * delta_r);
}

void Analyzer::compute_radial_probability(double radius, double center_x, double center_y)
{
    double d_r = radius * 2 / MAP_X;
    this->radial_probability_r = std::vector<double>((int)(radius / this->size_cell_x) + 1, 0);
    this->radial_probability_p = std::vector<double>((int)(radius / this->size_cell_y) + 1, 0);
    double integral = 0;
    for (int i = 0; i < MAP_X / 2; i++)
    {
        this->radial_probability_r[i] = delta_r + i * d_r;
        this->radial_probability_p[i] = this->count_occurrences(delta_r + i * d_r, center_x, center_y);
        integral += this->radial_probability_p[i];
    }
    integral *= d_r;
    for (int i = 0; i < MAP_X / 2; i++)
        this->radial_probability_p[i] /= integral;
}

double Analyzer::compute_near_wall_probability(double radius, double center_x, double center_y)
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