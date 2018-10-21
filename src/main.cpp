#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "simulation.h"
#include "analyzer.h"
#include "visualization.h"

int main(int argc, char *argv[])
{
    printf("Computing simulations and statistics...\n");
    int n_simulations = 1;
    double delta_time_step = 1e-5;
    int n_time_steps = (int)(25. / delta_time_step);
    double radius = 150.;

    gsl_rng_env_setup();
    const gsl_rng_type *random_generator_info;
    random_generator_info = gsl_rng_default;
    gsl_rng *random_generator = gsl_rng_alloc (random_generator_info);
    gsl_rng_set(random_generator, 314);

    Analyzer analyzer;

    for(int j=0; j<n_simulations; ++j)
    {
        printf("%d ", j);
        Simulation world(delta_time_step, n_time_steps, random_generator);
        for(int i=0; i<n_time_steps-1; ++i)
            world.compute_next_step();
        analyzer.compute_probability_map(&world, 0, n_time_steps, -radius, -radius, radius, radius);
    Visualization visualization;
    visualization.render(&world, 0, n_time_steps, 6);
    }

    analyzer.save_probability_map("probability_map.csv");

/*
    printf("Visualization...\n");
    Visualization visualization;
    visualization.render(&world, 0, n_time_steps, 6);
*/
    return 0;
}
