#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sstream>

#include "simulation.h"
#include "analyzer.h"
#include "visualization.h"

int main(int argc, char *argv[])
{
    double delta_time_step = 1e-4;
    int n_time_steps = (int)(100. / delta_time_step);

    gsl_rng_env_setup();
    const gsl_rng_type *random_generator_info;
    random_generator_info = gsl_rng_default;
    gsl_rng *random_generator = gsl_rng_alloc(random_generator_info);
    gsl_rng_set(random_generator, 3);

    std::array<double, 1> radius_list = {25.};//, 50., 75., 100., 125., 150., 250., 500.};
    for(double radius: radius_list)
    {
        printf("Radius: %d\n", (int)radius);
        printf("\tComputing simulations and probability map...\n");
        Analyzer analyzer;
        int n_simulations = 10;
        for (int j = 0; j < n_simulations; ++j)
        {
            printf("\tsimulation n %d...\n", j);
            Simulation world(delta_time_step, n_time_steps, random_generator, radius);
            for (int i = 0; i < n_time_steps - 1; ++i)
                world.compute_next_step();
            analyzer.compute_probability_map(&world, 0, n_time_steps, -radius, -radius, radius, radius);
        }

        printf("\tComputing radial probability...\n");
        analyzer.compute_radial_probability(radius, 0.0, 0.0);

        printf("\tComputing near-wall probability...  ");
        double near_wall = analyzer.compute_near_wall_probability(radius, 0.0, 0.0);
        printf("%f\n", near_wall);

        printf("\tSaving stuff...\n");
        std::stringstream strm;
        strm << "output/r_" << (int)radius << "_probability_map.csv";
        analyzer.save_probability_map(strm.str().c_str());
        strm.str("");
        strm << "output/r_" << (int)radius << "_radial_probability.csv";
        analyzer.save_radial_probability(strm.str().c_str());
    }

    // printf("Simulation...\n");
    // Simulation world(delta_time_step, n_time_steps, random_generator, radius_list[1]);
    //     for(int i=0; i<n_time_steps-1; ++i)
    //         world.compute_next_step();
    // printf("Visualization...\n");
    // Visualization visualization;
    // visualization.render(&world, 0, n_time_steps, 1);

    gsl_rng_free(random_generator);
    return 0;
}
