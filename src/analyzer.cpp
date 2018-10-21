#include "analyzer.h"
#include <fstream>

#include "bacterium.h"

Analyzer::Analyzer()
{
    this->reset_probability_map();
}

void Analyzer::reset_probability_map()
{
    for(int x=0; x<MAP_X; x++)
        for(int y=0; y<MAP_Y; y++)
            this->probability_map[x][y] = 0;
    this->n_map_points = 0;
}

void Analyzer::compute_probability_map(Simulation *world, int start_time_step, int end_time_step, double left_x, double top_y, double right_x, double bottom_y)
{
    double size_cell_x = (right_x - left_x)/MAP_X;
    double size_cell_y = (bottom_y - top_y)/MAP_Y;

    std::vector<double> bacterium_x = world->get_bacterium()->get_history_body_x(start_time_step, end_time_step);
    std::vector<double> bacterium_y = world->get_bacterium()->get_history_body_y(start_time_step, end_time_step);
    
    for(int i=0; i< end_time_step-start_time_step; i++)
        if(bacterium_x[i]>left_x && bacterium_x[i]<right_x && bacterium_y[i]>top_y && bacterium_y[i]<bottom_y)
            this->probability_map[(int)((bacterium_x[i]-left_x)/size_cell_x)][(int)((bacterium_y[i]-top_y)/size_cell_y)] += 1;
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