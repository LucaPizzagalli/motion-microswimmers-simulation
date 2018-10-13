#include <stdio.h>
#include "simulation.h"
#include "visualization.h"


int main(int argc, char *argv[])
{
    printf("Computing the simulation...\n");
    double delta_time_step = 0.0002;
    int n_time_steps = (int)(200 / delta_time_step);
    Simulation world(0.0002, n_time_steps);
    for(int i=0; i<n_time_steps-1; ++i)
        world.compute_next_step();

    printf("Visualization...\n");
    Visualization visualization;
    visualization.render(world, 0, n_time_steps, 6);

    return 0;
}

