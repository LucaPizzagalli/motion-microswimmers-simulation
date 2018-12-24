#ifndef DISKWALL_H
#define DISKWALL_H

#include "include/json.hpp"
#include "definition.hpp"
#include "bacterium.hpp"

class DiskWall
{
    bool throw_errors;

    double center_x;
    double center_y;
    double inner_radius;
    double outer_radius;
    double hardness;

public:
    DiskWall(nlohmann::json parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters);
    CellForce force_acting_on(Bacterium *bacterium, int *n_errors);
    void draw(int time_step, Camera *camera);
};

#endif
