#include <stdio.h>
#include "simulation.h"
#include "visualization.h"


int main(int argc, char *argv[])
{
    int n_time_steps = 1000;
    printf("Hello\n");
    Simulation world(1., n_time_steps);
    for(int i=0; i<n_time_steps-1; ++i)
        world.compute_next_step();

    Visualization visualization;
    visualization.render(world, 0, n_time_steps);

    return 0;
}

