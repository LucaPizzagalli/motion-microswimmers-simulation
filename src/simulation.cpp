#include "simulation.h"
#include <cmath>
#include <gsl/gsl_randist.h>

Simulation::Simulation(double delta_time_step, int total_time_steps)
{
    const gsl_rng_type *random_generator_info;
    
    gsl_rng_env_setup();
    random_generator_info = gsl_rng_default;
    this->random_generator = gsl_rng_alloc (random_generator_info);
    gsl_rng_set(this->random_generator, 44);

    this->disk_wall = new DiskWall();
    this->bacterium = new Bacterium(total_time_steps, gsl_ran_gaussian(random_generator, 10.), gsl_ran_gaussian(random_generator, 10.), gsl_ran_flat(this->random_generator, -M_PI, M_PI));

    this->delta_time_step = delta_time_step;
    this->time_step = 0;
}

void Simulation::compute_next_step()
{
    this->time_step++;
    Force force = this->disk_wall->force_acting_on(this->time_step, this->bacterium);
    this->bacterium->compute_step(this->time_step, this->delta_time_step, force, this->random_generator);
}

void Simulation::draw_frame(int time_step, Camera *camera)
{
    this->disk_wall->draw(time_step, camera);
    this->bacterium->draw(time_step, camera);
}

Simulation::~Simulation()
{
    // delete disk_wall;
    // delete bacterium;
    // gsl_rng_free(this->random_generator);
}