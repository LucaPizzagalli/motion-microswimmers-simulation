#include "simulation.h"
#include <gsl/gsl_randist.h>

Simulation::Simulation(double delta_time_step, int total_time_steps):
    bacterium(total_time_steps, 100., 100., 0.),
    disk_wall()
{
    const gsl_rng_type *random_generator_info;
    
    gsl_rng_env_setup();
    random_generator_info = gsl_rng_default;
    this->random_generator = gsl_rng_alloc (random_generator_info);
    gsl_rng_set(this->random_generator, 41);

    this->delta_time_step = delta_time_step;
    this->time_step = 0;
}

void Simulation::compute_next_step()
{
    this->time_step++;
    this->bacterium.compute_step(this->time_step, this->delta_time_step, this->random_generator);
}

void Simulation::draw_frame(int time_step, unsigned char screen_color[SCREEN_HEIGHT][SCREEN_WIDTH][4])
{
    this->disk_wall.draw(time_step, screen_color);
    this->bacterium.draw(time_step, screen_color);
}

Simulation::~Simulation()
{
    //gsl_rng_free(this->random_generator);
}