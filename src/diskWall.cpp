#include "diskWall.h"
#include <cmath>
#include <algorithm>

DiskWall::DiskWall()
{
    this->center_x = 0.;
    this->center_y = 0.;
    this->inside_radius = 100.; //25 50 100 150 500 micrometers
    this->outside_radius = this->inside_radius + 5.;
    this->epsilon = 10.; // who knows why
}

Force DiskWall::force_acting_on(int now, Bacterium *bacterium)
{
    double x, y;
    double body_e_x, body_e_y, force_body_modulus;
    double flagella_e_x, flagella_e_y, force_flagella_modulus;

    x = bacterium->get_body_x(now - 1) - this->center_x;
    y = bacterium->get_body_y(now - 1) - this->center_y;
    double body_distance = sqrt(x * x + y * y);
    if (body_distance > 0)
    {
        body_e_x = -x / body_distance;
        body_e_y = -y / body_distance;
        body_distance = this->inside_radius - body_distance;

        if (body_distance <= 0)
        {
            printf("Error body distance %d\n", now);
            throw "Error";
        }
        else if (body_distance < bacterium->get_body_radius(now - 1) * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(bacterium->get_body_radius(now - 1), 6.);
            double dist_6 = pow(body_distance, 6.);
            force_body_modulus = 24 * this->epsilon * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * body_distance) - rad_6 / (dist_6 * body_distance));
        }
        else
            force_body_modulus = 0;
    }
    else
    {
        body_e_x = 0;
        body_e_y = 0;
        force_body_modulus = 0;
    }

    x = bacterium->get_flagella_x(now - 1) - this->center_x;
    y = bacterium->get_flagella_y(now - 1) - this->center_y;

    double flagella_distance = sqrt(x * x + y * y);
    if (flagella_distance != 0)
    {
        flagella_e_x = -x / flagella_distance;
        flagella_e_y = -y / flagella_distance;
        flagella_distance = this->inside_radius - flagella_distance;
        if (flagella_distance <= 0)
        {
            printf("Error flagella distance %d\n", now);
            throw "Error";
        }
        else if (flagella_distance < bacterium->get_flagella_radius(now - 1) * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(bacterium->get_flagella_radius(now - 1), 6.);
            double dist_6 = pow(flagella_distance, 6.);
            force_flagella_modulus = 24 * this->epsilon * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * flagella_distance) - rad_6 / (dist_6 * flagella_distance));
            force_flagella_modulus = std::min(1000., force_flagella_modulus);
        }
        else
            force_flagella_modulus = 0;
    }
    else
    {
        flagella_e_x = 0;
        flagella_e_y = 0;
        force_flagella_modulus = 0;
    }

    return Force{force_body_modulus * body_e_x + force_flagella_modulus * flagella_e_x, force_body_modulus * body_e_y + force_flagella_modulus * flagella_e_y};
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
    double radius = this->outside_radius * camera->zoom;
    double middle_radius = (this->outside_radius + this->inside_radius) / 2 * camera->zoom;
    double thickness = (this->outside_radius - this->inside_radius) / 2 * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double radius = sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
            double fading = std::max(1 - (radius - middle_radius) * (radius - middle_radius) / (thickness * thickness), 0.);
            camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + 255 * fading);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
}