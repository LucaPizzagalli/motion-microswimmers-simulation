#include "diskWall.h"
#include <cmath>
#include <algorithm>

DiskWall::DiskWall()
{
    this->center_x = 0.;
    this->center_y = 0.;
    this->inside_radius = 50.; //25 50 100 150 500 micrometers
    this->outside_radius = this->inside_radius + 5.;
}
void DiskWall::draw(int time_step, Camera *camera)
{
    double center_x = (this->center_x - camera->x) * camera->zoom;
    double center_y = (this->center_y - camera->y) * camera->zoom;
    double radius =  this->outside_radius * camera->zoom;
    double middle_radius = (this->outside_radius + this->inside_radius)/2 * camera->zoom;
    double thickness = (this->outside_radius - this->inside_radius)/2 * camera->zoom;
    for(int x=(int)(center_x-radius); x<=(int)(center_x+radius)+1; x++)
        for(int y=(int)(center_y-radius); y<=(int)(center_y+radius)+1; y++)
        {
            double radius = sqrt((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y));
            double fading = std::max(1-(radius - middle_radius)*(radius - middle_radius)/(thickness*thickness), 0.);
		    camera->pixels[y][x][2] = int(camera->pixels[y][x][2]*(1-fading)+255*fading);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1]*(1-fading)+255*fading);
        }
}