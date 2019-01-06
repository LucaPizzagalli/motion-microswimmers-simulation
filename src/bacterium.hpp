#ifndef BACTERIUM_H
#define BACTERIUM_H

#include <gsl/gsl_rng.h>
#include <vector>

#include "include/json.hpp"
#include "definition.hpp"

class Bacterium
{
    bool throw_errors;
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
    double shear_time;

    double _sqrt_noise_torque_strength;
    double _sqrt_noise_force_strength;
    
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
    Bacterium(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator);
    void compute_step(int now, double delta_time_step, CellForce forces, int *n_errors);
    void update_state(int now);
    double get_body_radius();
    double get_flagella_radius();
    double get_body_x();
    double get_body_y();
    double get_flagella_x();
    double get_flagella_y();
    double get_history_body_x(int time_step);
    double get_history_body_y(int time_step);
    double get_history_flagella_x(int time_step);
    double get_history_flagella_y(int time_step);
    std::string state_to_string(int time_step = -1);
    void draw(int time_step, Camera *camera);

protected:
    double _compute_torque(CellForce forces, double sin_direction, double cos_direction);
    double _tumble(double delta_time_step);
    void _rotate(double rotation, double sin_direction, double cos_direction);
};

#endif
