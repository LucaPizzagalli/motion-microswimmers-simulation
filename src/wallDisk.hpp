#ifndef DISKWALL_H
#define DISKWALL_H

#include "nlohmann/json.hpp"
#include "definition.hpp"
#include "map.hpp"
#include "actor.hpp"
#include "cell.hpp"

class WallDisk: public Actor
{
    Vector2D coord;
    double inner_radius;
    double outer_radius;
    double hardness;

  public:
    WallDisk(nlohmann::json parameters, Map *map);
    Vector2D get_coord() const;
    double get_inner_radius();
    double get_hardness();
    CellForce interaction(Cell* cell, int now) override;
    std::string state_to_string(int time_step = -1) const;
    void draw(int time_step, Camera *camera) const;
};

#endif
