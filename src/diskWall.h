#ifndef DISKWALL_H
#define DISKWALL_H

#include <memory>
#include "definition.h"
#include "bacterium.h"

class DiskWall
{
    double center_x;
    double center_y;
    double inside_radius;
    double outside_radius;
    double epsilon;

public:
    DiskWall();
    Force force_acting_on(int now, Bacterium *bacterium);
    void draw(int time_step, Camera *camera);
};

#endif
