#ifndef DISKWALL_H
#define DISKWALL_H

#include "include/json.hpp"
#include "definition.hpp"
#include "actor.hpp"

struct WallInstance: public ActorInstance
{ };

class DiskWall: public Actor
{
    WallInstance instance;
    double inner_radius;
    double outer_radius;
    double hardness;

public:
    DiskWall(nlohmann::json parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters);
    void compute_step(int now, double delta_time_step, ActorForce force, int *n_errors) override;
    void update_state(int now) override;
    WallInstance* get_instance(int time_step) override;
    double get_inner_radius();
    double get_hardness();
    std::string state_to_string(int time_step = -1) override;
    void draw(int time_step, Camera *camera) override;
};

#endif
