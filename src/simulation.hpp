#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>
#include "include/json.hpp"
#include "definition.hpp"
#include "diskWall.hpp"
#include "bacterium.hpp"

class Simulation
{
    int n_errors;
    gsl_rng *random_generator;

    double delta_time_step;
    int n_time_steps;
    int time_step;
    int step_size;
    DiskWall disk_wall;
    Bacterium bacterium;

public:
    Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator);
    void compute_next_step();
    int compute_simulation();
    double get_delta_time_step();
    Bacterium* get_bacterium();
    void draw_frame(int time_step, Camera *camera);
};

#endif
