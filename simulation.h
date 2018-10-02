#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

struct Snapshot
{
    double theta; // orientation of the swimmer
    double x_position; // positions of the swimmer
    double y_position; // positions of the swimmer
};

class Simulation
{
    double delta_time_step;
    int time_step;
    std::vector<Snapshot> history;

public:
    Simulation(double delta_time_step, int total_time_steps);
    void compute_next_step();
    Snapshot get_snapshot(int time_step);
};

#endif
