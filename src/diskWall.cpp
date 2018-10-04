#include "diskWall.h"
#include <cmath>
#include <algorithm>

DiskWall::DiskWall()
{
    this->center_x = SCREEN_WIDTH/2;
    this->center_y = SCREEN_HEIGHT/2;
    this->inside_radius = SCREEN_HEIGHT/3;
    this->outside_radius = SCREEN_HEIGHT/3 + 10;
}
void DiskWall::draw(int time_step, unsigned char screen_color[SCREEN_HEIGHT][SCREEN_WIDTH][4])
{
    double middle_radius = (this-> outside_radius + this->inside_radius)/2;
    double thickness = (this-> outside_radius - this->inside_radius)/2;
    for(int x=(int)(this->center_x-this->outside_radius); x<=(int)(this->center_x+this->outside_radius)+1; x++)
        for(int y=(int)(this->center_y-this->outside_radius); y<=(int)(this->center_y+this->outside_radius)+1; y++)
        {
            double radius = sqrt((x-this->center_x)*(x-this->center_x)+(y-this->center_y)*(y-this->center_y));
            double fading = std::max(1-(radius - middle_radius)*(radius - middle_radius)/(thickness*thickness), 0.);
		    screen_color[y][x][2] = int(screen_color[y][x][2]*(1-fading)+255*fading);
            screen_color[y][x][1] = int(screen_color[y][x][1]*(1-fading)+255*fading);
        }
}