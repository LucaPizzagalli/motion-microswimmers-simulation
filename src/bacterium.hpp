#ifndef BACTERIUM_H
#define BACTERIUM_H

#include <gsl/gsl_rng.h>
#include <vector>

#include "include/json.hpp"
#include "definition.hpp"

class Bacterium
{
    gsl_rng *random_generator;
    int step_size;

    double body_radius;
    double flagella_radius;
    double body_flagella_distance;
    double rotation_center;

    double speed;
    double tumble_delay_mean;
    double tumble_strength_mean;
    double tumble_strength_std;
    double tumble_duration_mean;
    double tumble_duration_std;

    double diffusivity, _sqrt_diffusivity;
    double persistence_time, _sqrt_persistence_time;
    double shear_time;
    
    double prev_center_x, next_center_x;
    double prev_center_y, next_center_y;
    double prev_direction, next_direction;
    double prev_tumble_countdown, next_tumble_countdown;
    double prev_tumble_speed, next_tumble_speed;
    double prev_tumble_duration, next_tumble_duration;

    std::vector<double> center_x;
    std::vector<double> center_y;
    std::vector<double> direction;
    std::vector<double> tumble_countdown;
    std::vector<double> tumble_speed;
    std::vector<double> tumble_duration;

public:
    Bacterium(nlohmann::json parameters, nlohmann::json initial_conditions, int total_time_steps, int step_size, gsl_rng *random_generator);
    void compute_step(int now, double delta_time_step, CellForce forces);
    void update_state(int now);
    double get_body_radius();
    double get_flagella_radius();
    double get_body_x();
    double get_body_y();
    double get_history_body_x(int time_step);
    double get_history_body_y(int time_step);
    double get_flagella_x();
    double get_flagella_y();
    std::string state_to_string();
    void draw(int time_step, Camera *camera);

protected:
    double _compute_torque(CellForce forces, double sin_direction, double cos_direction);
    double _tumble(double delta_time_step);
    void _rotate(double rotation, double sin_direction, double cos_direction);
};

#endif