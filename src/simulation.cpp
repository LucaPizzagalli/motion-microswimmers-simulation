#include "simulation.h"
#include <cmath>
#include <gsl/gsl_randist.h>

Simulation::Simulation(double delta_time_step, int total_time_steps, gsl_rng *random_generator, double radius):
    disk_wall (radius),
    bacterium (total_time_steps, 0., 0., 4/*gsl_ran_flat(random_generator, -M_PI, M_PI)*/)
{
    this->random_generator = random_generator;

    this->delta_time_step = delta_time_step;
    this->time_step = 0;
}

void Simulation::compute_next_step()
{
    this->time_step++;
    Force force = this->disk_wall.force_acting_on(this->time_step, &(this->bacterium));
    this->bacterium.compute_step(this->time_step, this->delta_time_step, force, this->random_generator);
}

Bacterium* Simulation::get_bacterium()
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