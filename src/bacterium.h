#ifndef BACTERIUM_H
#define BACTERIUM_H

#include "definition.h"
#include <gsl/gsl_rng.h>
#include <vector>

class Bacterium
{
    double body_radius;
    double flagella_radius;
    std::vector<double> x_position; // positions of the swimmer
    std::vector<double> y_position; // positions of the swimmer
    std::vector<double> theta; // orientation of the swimmer

public:
    Bacterium(int total_time_steps, double x_position, double y_position, double theta);
    void compute_step(int now, double delta_time_step, gsl_rng *random_generator);
    void draw(int time_step, Camera *camera);
};

#endif
