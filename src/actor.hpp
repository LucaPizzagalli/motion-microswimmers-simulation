#ifndef ACTOR_H
#define ACTOR_H

#include <gsl/gsl_rng.h>
#include <sstream>

#include "definition.hpp"

struct ActorForce
{
    Vector2D coord[2];

    ActorForce(Vector2D coord1 = {0., 0.}, Vector2D coord2 = {0., 0.})
    {
        coord[0] = coord1;
        coord[1] = coord2;
    }

    ActorForce operator+(const ActorForce &other) const
    {
        return ActorForce(this->coord[0] + other[0], this->coord[1] + other[1]);
    }
    void operator+=(const ActorForce &other)
    {
        this->coord[0] += other[0];
        this->coord[1] += other[1];
    }
    Vector2D operator[](const int index) const
    {
        return this->coord[index];
    }
};

class Actor
{
  public:
    virtual void compute_step(int now, double delta_time_step, ActorForce force, int *n_errors) = 0;
    virtual void update_state(int now) = 0;
    virtual std::string state_to_string(int time_step) const = 0;
    virtual void draw(int time_step, Camera *camera) const = 0;
    virtual ~Actor() { }
};

#endif
