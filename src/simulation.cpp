#include "simulation.h"
#include <gsl/gsl_randist.h>

Simulation::Simulation(double delta_time_step, int total_time_steps):
    bacterium(total_time_steps, 0., 0., 0.),
    disk_wall()
{
    const gsl_rng_type *random_generator_info;
    
    gsl_rng_env_setup();
    random_generator_info = gsl_rng_default;
    this->random_generator = gsl_rng_alloc (random_generator_info);
    gsl_rng_set(this->random_generator, 44);

    this->delta_time_step = delta_time_step;
    this->time_step = 0;
}

void Simulation::compute_next_step()
{
    this->time_step++;
    Force force = this->disk_wall.force_acting_on(this->time_step, this->bacterium);
    this->bacterium.compute_step(this->time_step, this->delta_time_step, force, this->random_generator);
}

void Simulation::draw_frame(int time_step, Camera *camera)
{
    this->disk_wall.draw(time_step, camera);
    this->bacterium.draw(time_step, camera);
}

Simulation::~Simulation()
{
    //gsl_rng_free(this->random_generator);
}