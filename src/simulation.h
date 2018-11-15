#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>
#include "definition.h"
#include "diskWall.h"
#include "bacterium.h"

class Simulation
{
    gsl_rng *random_generator;

    double delta_time_step;
    int time_step;
    DiskWall disk_wall;
    Bacterium bacterium;

public:
    Simulation(double delta_time_step, int total_time_steps, gsl_rng *random_generator, double radius);
    void compute_next_step();
    double get_delta_time_step();
    Bacterium* get_bacterium();
    void draw_frame(int time_step, Camera *camera);
};

#endif
