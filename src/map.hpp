#ifndef MAP_H
#define MAP_H

#include <vector>
#include <set>
#include "definition.hpp"
#include "actor.hpp"

class Map
{
    std::vector<std::set<Actor *>> cell;
    double cell_size;
    double top;
    double left;
    int height;
    int width;
    bool isMapping;

    public:
    Map(double top, double bottom, double left, double right, double cell_size);

    void depart(Actor* actor, Vector2D coord);
    void arrive(Actor* actor, Vector2D coord);
    std::set<Actor *> check(Actor* actor, Vector2D coord);
    void horizontal(Actor *actor, double yy);
    void vertical(Actor *actor, double xx);
    std::string to_string() const;
};

#endif