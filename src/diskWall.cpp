#include "diskWall.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

DiskWall::DiskWall(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters)
{
    this->inner_radius = physics_parameters["innerRadius"].get<double>();
    this->outer_radius = this->inner_radius + physics_parameters["thickness"].get<double>();
    this->hardness = physics_parameters["wallInteraction"]["hardness"].get<double>();

    this->center_x = initial_conditions["position"]["x"].get<double>();
    this->center_y = initial_conditions["position"]["y"].get<double>();
}

CellForce DiskWall::force_acting_on(Bacterium *bacterium)
{
    double x, y;
    double body_e_x, body_e_y, force_body_modulus;
    double flagella_e_x, flagella_e_y, force_flagella_modulus;

    // force on body
    x = bacterium->get_body_x() - this->center_x;
    y = bacterium->get_body_y() - this->center_y;
    double body_distance = sqrt(x * x + y * y);
    if (body_distance > 0)
    {
        body_e_x = -x / body_distance;
        body_e_y = -y / body_distance;
        body_distance = this->inner_radius - body_distance;

        if (body_distance <= 0)
            force_body_modulus = 10000.;
        else if (body_distance < bacterium->get_body_radius() * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(bacterium->get_body_radius(), 6.);
            double dist_6 = pow(body_distance, 6.);
            force_body_modulus = 24 * this->hardness * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * body_distance) - rad_6 / (dist_6 * body_distance));
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

    // force on flagella
    x = bacterium->get_flagella_x() - this->center_x;
    y = bacterium->get_flagella_y() - this->center_y;

    double flagella_distance = sqrt(x * x + y * y);
    if (flagella_distance != 0)
    {
        flagella_e_x = -x / flagella_distance;
        flagella_e_y = -y / flagella_distance;
        flagella_distance = this->inner_radius - flagella_distance;
        if (flagella_distance <= 0)
            force_flagella_modulus = 10000.;
        else if (flagella_distance < bacterium->get_flagella_radius() * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(bacterium->get_flagella_radius(), 6.);
            double dist_6 = pow(flagella_distance, 6.);
            force_flagella_modulus = 24 * this->hardness * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * flagella_distance) - rad_6 / (dist_6 * flagella_distance));
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
    return CellForce{force_body_modulus * body_e_x, force_body_modulus * body_e_y, force_flagella_modulus * flagella_e_x, force_flagella_modulus * flagella_e_y};
}

void DiskWall::draw(int time_step, Camera *camera)
{
    double center_x = (this->center_x - camera->x) * camera->zoom;
    double center_y = (this->center_y - camera->y) * camera->zoom;
    double radius = this->outer_radius * camera->zoom;
    double middle_radius = (this->outer_radius + this->inner_radius) / 2 * camera->zoom;
    double thickness = (this->outer_radius - this->inner_radius) / 2 * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double radius = sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
            double fading = std::max(1 - (radius - middle_radius) * (radius - middle_radius) / (thickness * thickness), 0.);
            camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + 255 * fading);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
}