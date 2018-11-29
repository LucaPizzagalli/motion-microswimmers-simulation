#ifndef BACTERIUM_H
#define BACTERIUM_H

#include <gsl/gsl_rng.h>
#include <vector>

#include "include/json.hpp"
#include "definition.hpp"

class Bacterium
{
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
    
    std::vector<double> center_x; // positions of the swimmer
    std::vector<double> center_y; // positions of the swimmer
    std::vector<double> direction; // orientation of the swimmer
    std::vector<double> tumble_countdown; // time left before next tumble
    std::vector<double> tumble_speed;
    std::vector<double> tumble_duration;

public:
    Bacterium(nlohmann::json parameters, nlohmann::json initial_conditions, int total_time_steps);
    void compute_step(int now, double delta_time_step, CellForce forces, gsl_rng *random_generator);
    double get_body_radius(int time_step);
    double get_flagella_radius(int time_step);
    double get_body_x(int time_step);
    double get_body_y(int time_step);
    double get_flagella_x(int time_step);
    double get_flagella_y(int time_step);
    std::vector<double> get_history_body_x(int start_time_step, int end_time_step);
    std::vector<double> get_history_body_y(int start_time_step, int end_time_step);
    void draw(int time_step, Camera *camera);

protected:
    double _compute_torque(CellForce forces, double sin_direction, double cos_direction);
    double _tumble(double now, double delta_time_step, gsl_rng *random_generator);
    void _rotate(double now, double rotation, double sin_direction, double cos_direction);
};

#endif
