#ifndef WALLLEFT_H
#define WALLLEFT_H

#include "nlohmann/json.hpp"
#include "definition.hpp"
#include "map.hpp"
#include "actor.hpp"
#include "cell.hpp"

class WallLeft: public Actor
{
    double x, x2;
    double hardness;

public:
    WallLeft(nlohmann::json parameters, Map *map);
    double get_x() const;
    double get_hardness() const;
    CellForce interaction(Cell* cell, int now) override;
    std::string state_to_string(int time_step = -1) const;
    void draw(int time_step, Camera *camera) const;
};

#endif
