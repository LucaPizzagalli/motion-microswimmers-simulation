#ifndef DISKWALL_H
#define DISKWALL_H

#include "definition.h"

class DiskWall
{
    double center_x;
    double center_y;
    double inside_radius;
    double outside_radius;

public:
    DiskWall();
    void draw(int time_step, Camera *camera);
};

#endif
