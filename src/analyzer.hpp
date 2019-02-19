#ifndef ANALYZER_H
#define ANALYZER_H

#include "definition.hpp"
#include "simulation.hpp"
#include <array>

class Analyzer
{
    bool map_stats;
    bool displacement_stats;
    std::vector <std::vector <double> > probability_map;
    std::vector <double> radial_probability_p;
    std::vector <double> radial_probability_r;
    std::vector <double> displacement;
    double wall_radius;
    int map_width;
    int map_height;
    double probability_map_left_corner_x;
    double probability_map_top_corner_y;
    double probability_map_right_corner_x;
    double probability_map_bottom_corner_y;
    int n_map_points;
    int n_tracks;
    double size_cell_x;
    double size_cell_y;
    double near_wall_probability;
    double time_step_size;

public:
    Analyzer(nlohmann::json simulation_parameters, nlohmann::json physics_parameters);
    void update_stats(Simulation *world, int start_time_step, int end_time_step, int step_size);
    void compute_stats();
    void compute_radial_probability(double center_x, double center_y);
    void compute_near_wall_probability();
    void compute_displacement();
    void save_stats(const std::string &file_name);
    void save_probability_map(const std::string &file_name);
    void save_radial_probability(const std::string &file_name);
    void save_near_wall_probability(const std::string &file_name);
    void save_displacement(const std::string &file_name);
};

#endif
