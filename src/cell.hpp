#ifndef CELL_H
#define CELL_H

#include <gsl/gsl_rng.h>
#include <vector>

#include "nlohmann/json.hpp"
#include "definition.hpp"
#include "actor.hpp"
#include "map.hpp"

struct CellInstance
{
    Vector2D coord;
    double direction;
    double tumble_countdown;
    double tumble_speed;
    double tumble_duration;

    CellInstance(Vector2D coord = {0., 0.}, double direction = 0., double tumble_countdown = 0., double tumble_speed = 0., double tumble_duration = 0.)
        : coord(coord), direction(direction), tumble_countdown(tumble_countdown), tumble_speed(tumble_speed), tumble_duration(tumble_duration)
    {
    }
};

class Cell: public Actor
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

    double _body_body_6, _body_flagella_6, _flagella_flagella_6;

  public:
    Cell(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator, Map *map);
    void compute_step(int now, double delta_time_step, CellForce force, int *n_errors);
    void update_state(int now, Map *map);
    double get_body_radius() const;
    double get_flagella_radius() const;
    Vector2D get_flagella_coord(CellInstance instance) const;
    CellInstance get_instance(int time_step) const;
    CellForce interaction(Cell* cell, int now) override;
    std::string state_to_string(int time_step) const;
    void draw(int time_step, Camera *camera) const;

  protected:
    double _compute_torque(CellForce force, Vector2D e_direction);
    double _tumble(double delta_time_step);
    void _rotate(double rotation, Vector2D e_direction);
};

#endif
