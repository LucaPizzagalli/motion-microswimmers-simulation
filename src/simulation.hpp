#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>
#include "include/json.hpp"
#include "definition.hpp"
#include "diskWall.hpp"
#include "bacterium.hpp"

class Simulation
{
    gsl_rng *random_generator;

    double delta_time_step;
    int time_step;
    DiskWall disk_wall;
    Bacterium bacterium;

public:
    Simulation(nlohmann::json parameters, nlohmann::json initial_conditions, double delta_time_step, int total_time_steps, int step_size, gsl_rng *random_generator);
    void compute_next_step();
    double get_delta_time_step();
    Bacterium* get_bacterium();
    void draw_frame(int time_step, Camera *camera);
};

#endif