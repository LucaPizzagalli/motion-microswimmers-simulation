#include <stdio.h>
#include "simulation.h"
#include "visualization.h"


int main(int argc, char *argv[])
{
    double delta_time_step = 0.0002;
    int n_time_steps = (int)(4 / delta_time_step);
    printf("Hello\n");
    Simulation world(0.0002, n_time_steps);
    for(int i=0; i<n_time_steps-1; ++i)
        world.compute_next_step();

    Visualization visualization;
    visualization.render(world, 0, n_time_steps, 10);

    return 0;
}

