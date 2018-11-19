#ifndef ANALYZER_H
#define ANALYZER_H

#include "definition.h"
#include "simulation.h"
#include <array>

class Analyzer
{
    std::vector <std::vector <double> > probability_map;
    std::vector <double> radial_probability_p;
    std::vector <double> radial_probability_r;
    int n_map_points;
    double size_cell_x;
    double size_cell_y;

public:
    Analyzer();
    void reset_probability_map();
    void compute_probability_map(Simulation *world, int start_time_step, int end_time_step, double left_x, double top_y, double right_x, double bottom_y);
    void compute_radial_probability(double radius, double center_x, double center_y);
    double compute_near_wall_probability(double radius, double center_x, double center_y);
    void save_probability_map(const std::string &file_name);
    void save_radial_probability(const std::string &file_name);
private:
    double count_occurrences(double radius, double center_x, double center_y, double delta_r);
};

#endif
