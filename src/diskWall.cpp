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

Force DiskWall::force_acting_on(int now, Bacterium bacterium)
{
    double body_distance = inside_radius - sqrt(bacterium.getBodyX(now-1)*bacterium.getBodyX(now-1)+bacterium.getBodyY(now-1)*bacterium.getBodyY(now-1));
    double body_e_x = -bacterium.getBodyX(now-1)/body_distance;
    double body_e_y = -bacterium.getBodyY(now-1)/body_distance;
    double flagella_distance = inside_radius - sqrt(bacterium.getFlagellaX(now-1)*bacterium.getFlagellaX(now-1)+bacterium.getFlagellaY(now-1)*bacterium.getFlagellaY(now-1));
    double flagella_e_x = -bacterium.getFlagellaX(now-1)/flagella_distance;
    double flagella_e_y = -bacterium.getFlagellaY(now-1)/flagella_distance;
    double force_body_modulus = 24 * 10. * 250. * (-2*pow(bacterium.getBodyRadius(now-1), 12.)/pow(body_distance, 13.) + pow(bacterium.getBodyRadius(now-1), 6.)/pow(body_distance, 7.) );
    double force_flagella_modulus = 24 * 10. * 250. * (-2*pow(bacterium.getFlagellaRadius(now-1), 12.)/pow(flagella_distance, 13.) + pow(bacterium.getFlagellaRadius(now-1), 6.)/pow(flagella_distance, 7.) );
    Force force = {force_body_modulus*body_e_x+force_flagella_modulus*flagella_e_x, force_body_modulus*body_e_y+force_flagella_modulus*flagella_e_y};
    printf("d: %f %f\n",body_distance,flagella_distance);
    printf("f: %f %f\n",force_body_modulus,force_flagella_modulus);
    printf("%f %f\n",force.x,force.y);
    return force;
}
// potential:
// eta = 10

// F_tot = - gradient(U_tot)
// U_tot = U_flagella + U_body
// U_flagella = k_b * T * eta * (4 * ((flagella_radius/distance)^12 - (flagella_radius/distance)^6) + 1)
// U_body = k_b * T * eta * (4 * ((body_radius/distance)^12 - (body_radius/distance)^6) + 1)

// torque:
// T_tot = T_flagella + T_body
// T_flagella = flagella_radius * (e cross F_flagella)/2
// T_body = -flagella_radius * (e cross F_body)/2

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