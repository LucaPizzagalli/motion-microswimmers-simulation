#ifndef DISKWALL_H
#define DISKWALL_H

#include "include/json.hpp"
#include "definition.hpp"
#include "bacterium.hpp"

class DiskWall
{
    double center_x;
    double center_y;
    double inner_radius;
    double outer_radius;
    double hardness;

public:
    DiskWall(nlohmann::json parameters, nlohmann::json initial_conditions);
    CellForce force_acting_on(Bacterium *bacterium);
    void draw(int time_step, Camera *camera);
};

#endif
