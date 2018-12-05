#ifndef ANALYZER_H
#define ANALYZER_H

#include "definition.hpp"
#include "simulation.hpp"
#include <array>

class Analyzer
{
    std::vector <std::vector <double> > probability_map;
    std::vector <double> radial_probability_p;
    std::vector <double> radial_probability_r;
    double probability_map_left_corner_x;
    double probability_map_top_corner_y;
    double probability_map_right_corner_x;
    double probability_map_bottom_corner_y;
    int n_map_points;
    double size_cell_x;
    double size_cell_y;

public:
    Analyzer(double left_x, double top_y, double right_x, double bottom_y);
    void update_probability_map(Simulation *world, int start_time_step, int end_time_step, int step_size);
    void compute_radial_probability(double radius, double center_x, double center_y);
    double compute_near_wall_probability(double radius);
    void save_probability_map(const std::string &file_name);
    void save_radial_probability(const std::string &file_name);
};

#endif
