#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>
#include "nlohmann/json.hpp"
#include "definition.hpp"
#include "diskWall.hpp"
#include "cell.hpp"

struct ForceCouple
{
    ActorForce a;
    ActorForce b;
};

class Simulation
{
    int n_errors;
    gsl_rng *random_generator;

    double delta_time_step;
    int n_time_steps;
    int time_step;
    int step_size;

    std::vector<std::shared_ptr<Actor>> actor;

public:
    Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator);
    void compute_next_step();
    int compute_simulation();
    ForceCouple interaction(std::shared_ptr<Actor> actor1, std::shared_ptr<Actor> actor2);
    double get_delta_time_step();
    std::vector<std::shared_ptr<Actor>> get_actors();
    void draw_frame(int time_step, Camera *camera);
};

#endif