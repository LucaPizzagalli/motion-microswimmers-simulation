#ifndef ANALYZER_H
#define ANALYZER_H

#include "definition.h"
#include "simulation.h"
#include <array>

class Analyzer
{
    std::vector <std::vector <double> > probability_map;
    int n_map_points;

public:
    Analyzer();
    void reset_probability_map();
    void compute_probability_map(Simulation *world, int start_time_step, int end_time_step, double left_x, double top_y, double right_x, double bottom_y);
    void save_probability_map(const std::string &file_name);
};

#endif
