#include "simulation.h"
#include "visualization.h"


int main(int argc, char *argv[])
{
    printf("Ciao\n");
    Simulation world(1., 20);
    for(int i=0; i<10; i++)
        world.compute_next_step();

    Visualization visualization;
    visualization.render(world, 0, 10);

    return 0;
}

