#ifndef ACTOR_H
#define ACTOR_H

#include <gsl/gsl_rng.h>
#include <sstream>

#include "include/json.hpp"
#include "definition.hpp"

struct ActorInstance
{
    double x;
    double y;

    ActorInstance(double x = 0., double y = 0.)
        : x(x), y(y)
    {}
};

struct ActorForce
{
    double x;
    double y;
    double x2;
    double y2;

    ActorForce(double x = 0., double y = 0., double x2 = 0., double y2 = 0.)
        : x(x), y(y), x2(x2), y2(y2)
    {}

    ActorForce operator+(const ActorForce &other) const
    {
        return ActorForce(x + other.x, y + other.y, x2 + other.x2, y2 + other.y2);
    }
    void operator+=(const ActorForce &other)
    {
        this->x += other.x;
        this->y += other.y;
        this->x2 += other.x2;
        this->y2 += other.y2;
    }
};

class Actor
{
  public:
    virtual void compute_step(int now, double delta_time_step, ActorForce force, int *n_errors) = 0;
    virtual void update_state(int now) = 0;
    virtual ActorInstance* get_instance(int time_step) = 0;
    virtual std::string state_to_string(int time_step) = 0;
    virtual void draw(int time_step, Camera *camera) = 0;
    virtual ~Actor() { }
};

#endif
