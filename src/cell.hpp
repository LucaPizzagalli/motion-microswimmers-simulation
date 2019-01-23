#ifndef CELL_H
#define CELL_H

#include <gsl/gsl_rng.h>
#include <vector>

#include "include/json.hpp"
#include "definition.hpp"
#include "actor.hpp"

struct CellInstance : public ActorInstance
{
    double direction;
    double tumble_countdown;
    double tumble_speed;
    double tumble_duration;

    CellInstance(double x = 0., double y = 0., double direction = 0., double tumble_countdown = 0., double tumble_speed = 0., double tumble_duration = 0.)
        : ActorInstance(x, y), direction(direction), tumble_countdown(tumble_countdown), tumble_speed(tumble_speed), tumble_duration(tumble_duration)
    { }
};

class Cell : public Actor
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

    CellInstance prev_instance;
    CellInstance next_instance;

    std::vector<CellInstance> instance;
    std::vector<double> tumble_countdown;
    std::vector<double> tumble_speed;
    std::vector<double> tumble_duration;

  public:
    Cell(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator);
    void compute_step(int now, double delta_time_step, ActorForce force, int *n_errors) override;
    void update_state(int now) override;
    double get_body_radius();
    double get_flagella_radius();
    double get_flagella_x(CellInstance *instance);
    double get_flagella_y(CellInstance *instance);
    virtual CellInstance *get_instance(int time_step) override;
    std::string state_to_string(int time_step) override;
    void draw(int time_step, Camera *camera) override;

  protected:
    double _compute_torque(ActorForce force, double sin_direction, double cos_direction);
    double _tumble(double delta_time_step);
    void _rotate(double rotation, double sin_direction, double cos_direction);
};

#endif
