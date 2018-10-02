#include "simulation.h"
#include <cmath>

Simulation::Simulation(double delta_time_step, int total_time_steps)
{
    this->delta_time_step = delta_time_step;
    this->history = std::vector<Snapshot>(total_time_steps);
    this->time_step = 0;
    this->history[0].x_position = 100.;
    this->history[0].y_position = 100.;
    this->history[0].theta = 10.;
}

void Simulation::compute_next_step()
{
    double vel_x, vel_y;
    Snapshot *now = &(this->history[this->time_step]);
    Snapshot *next = &(this->history[this->time_step+1]);
    // generate random numbers using BOOST library
    // boost::random::normal_distribution<> dist(0, 1);

    // euler scheme
    next->theta = now->theta + (0.01)*this->delta_time_step;
    vel_x = cos(now->theta);
    vel_y = sin(now->theta);
    next->x_position = now->x_position + vel_x*this->delta_time_step;
    next->y_position = now->y_position + vel_y*this->delta_time_step;
    this->time_step++;
}

Snapshot Simulation::get_snapshot(int time_step)
{
    return this->history[time_step];
}