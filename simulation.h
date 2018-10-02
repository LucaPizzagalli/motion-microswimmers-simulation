#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

class Simulation
{
    double delta_time_step;
    int time_step;
    std::vector<double> theta; // orientation of the swimmer
    std::vector<double> x_position; // positions of the swimmer
    std::vector<double> y_position; // positions of the swimmer

public:
    Simulation(double delta_time_step, int total_time_steps);
    void compute_next_step();
};

#endif
