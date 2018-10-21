#ifndef BACTERIUM_H
#define BACTERIUM_H

#include "definition.h"
#include <gsl/gsl_rng.h>
#include <vector>

class Bacterium
{
    double body_radius;
    double flagella_radius;
    double speed;
    double tumble_mean_time;
    double tumble_mean_strength;
    double tumble_std_strength;
    
    std::vector<double> center_x; // positions of the swimmer
    std::vector<double> center_y; // positions of the swimmer
    std::vector<double> direction; // orientation of the swimmer
    std::vector<double> tumble_countdown; // time left before next tumble

public:
    Bacterium(int total_time_steps, double x_position, double y_position, double direction);
    void compute_step(int now, double delta_time_step, Force force, gsl_rng *random_generator);
    double get_body_radius(int time_step);
    double get_flagella_radius(int time_step);
    double get_body_x(int time_step);
    double get_body_y(int time_step);
    double get_flagella_x(int time_step);
    double get_flagella_y(int time_step);
    std::vector<double> get_history_body_x(int start_time_step, int end_time_step);
    std::vector<double> get_history_body_y(int start_time_step, int end_time_step);
    void draw(int time_step, Camera *camera);
};

#endif
