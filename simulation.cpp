#include "simulation.h"
#include <cmath>

Simulation::Simulation(double delta_time_step, int total_time_steps)
{
    this->delta_time_step = delta_time_step;
    this->x_position = std::vector<double>(total_time_steps, 0.0);
    this->y_position = std::vector<double>(total_time_steps, 0.0);
    this->theta = std::vector<double>(total_time_steps, 0.0);
    this->time_step = 0;
    this->x_position[0] = 10.;
    this->y_position[0] = 10.;
}

void Simulation::compute_next_step()
{
    double vel_x, vel_y;
    // generate random numbers using BOOST library
    // boost::random::normal_distribution<> dist(0, 1);

    // euler scheme
    this->theta[this->time_step+1] = this->theta[this->time_step] + (0.01)*this->delta_time_step;
    vel_x = cos(theta[this->time_step]);
    vel_y = sin(theta[this->time_step]);
    x_position[this->time_step+1] = x_position[this->time_step] + vel_x*this->delta_time_step;
    y_position[this->time_step+1] = y_position[this->time_step] + vel_y*this->delta_time_step;
    this->time_step++;
}
