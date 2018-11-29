#include "simulation.hpp"
#include <cmath>
#include <gsl/gsl_randist.h>

Simulation::Simulation(nlohmann::json parameters, nlohmann::json initial_conditions, double delta_time_step, int total_time_steps, gsl_rng *random_generator) :
    disk_wall(parameters["wall"], initial_conditions["wall"]),
    bacterium(parameters["cell"], initial_conditions["cell"], total_time_steps)
{
    this->random_generator = random_generator;

    this->delta_time_step = delta_time_step;
    this->time_step = 0;
}

void Simulation::compute_next_step()
{
    this->time_step++;
    CellForce forces = this->disk_wall.force_acting_on(this->time_step, &(this->bacterium));
    this->bacterium.compute_step(this->time_step, this->delta_time_step, forces, this->random_generator);
}

Bacterium *Simulation::get_bacterium()
{
    return &(this->bacterium);
}

double Simulation::get_delta_time_step()
{
    return this->delta_time_step;
}

void Simulation::draw_frame(int time_step, Camera *camera)
{
    this->disk_wall.draw(time_step, camera);
    this->bacterium.draw(time_step, camera);
}