#ifndef ACTOR_H
#define ACTOR_H

#include <gsl/gsl_rng.h>
#include <sstream>

#include "definition.hpp"

class Cell;

class Actor
{
  public:
    virtual CellForce interaction(Cell* cell, int now) = 0;
    virtual ~Actor() { }
};

#endif