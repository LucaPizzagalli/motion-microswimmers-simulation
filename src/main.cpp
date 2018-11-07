#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "simulation.h"
#include "analyzer.h"
#include "visualization.h"

int main(int argc, char *argv[])
{
    printf("Computing simulations and statistics...\n");
    double delta_time_step = 1e-4;
    int n_time_steps = (int)(100. / delta_time_step);

    gsl_rng_env_setup();
    const gsl_rng_type *random_generator_info;
    random_generator_info = gsl_rng_default;
    gsl_rng *random_generator = gsl_rng_alloc (random_generator_info);
    gsl_rng_set(random_generator, 3);

    // Analyzer analyzer;
    // int n_simulations = 10;
    // double radius = 50.;
    // for(int j=0; j<n_simulations; ++j)
    // {
    //     printf("%d\n", j);
    //     Simulation world(delta_time_step, n_time_steps, random_generator);
    //     for(int i=0; i<n_time_steps-1; ++i)
    //         world.compute_next_step();
    //     analyzer.compute_probability_map(&world, 0, n_time_steps, -radius, -radius, radius, radius);
    //     if(j==4){
    //         Visualization visualization;
    //         visualization.render(&world, 0, n_time_steps, 1);
    //     }
    // }

    // analyzer.save_probability_map("probability_map.csv");



    printf("Simulation...\n");
    Simulation world(delta_time_step, n_time_steps, random_generator);
        for(int i=0; i<n_time_steps-1; ++i)
            world.compute_next_step();
    printf("Visualization...\n");
    Visualization visualization;
    visualization.render(&world, 0, n_time_steps, 1);

    return 0;
}
