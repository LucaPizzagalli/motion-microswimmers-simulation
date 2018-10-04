#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>
#include "definition.h"
#include "bacterium.h"

class Simulation
{
    gsl_rng *random_generator;

    double delta_time_step;
    int time_step;
    Bacterium bacterium;

public:
    Simulation(double delta_time_step, int total_time_steps);
    void compute_next_step();
    void draw_frame(int time_step, unsigned char screen_color[SCREEN_HEIGHT][SCREEN_WIDTH][4]);
    ~Simulation();
};

#endif
