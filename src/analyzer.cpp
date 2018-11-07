#include "analyzer.h"
#include <fstream>

#include "bacterium.h"

#define MAP_X 1000
#define MAP_Y 1000

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
    double size_cell_x = (right_x - left_x)/MAP_X;
    double size_cell_y = (bottom_y - top_y)/MAP_Y;
    
    for(int time=start_time_step; time<end_time_step; time++)
    {
        double x = world->get_bacterium()->get_body_x(time);
        double y = world->get_bacterium()->get_body_y(time);
        if(x>left_x && x<right_x && y>top_y && y<bottom_y)
            this->probability_map[(int)((x-left_x)/size_cell_x)][(int)((y-top_y)/size_cell_y)]++;
    }
    this->n_map_points += end_time_step-start_time_step;
}

void Analyzer::save_probability_map(const std::string &file_name)
{
    std::ofstream out(file_name);
    for (auto& row : this->probability_map)
    {
        for (double col : row)
            out << col/this->n_map_points <<",";
        out << "\n";
    }
}