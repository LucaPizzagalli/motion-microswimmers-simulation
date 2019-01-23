#include "diskWall.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

DiskWall::DiskWall(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters)
{
    this->inner_radius = physics_parameters["innerRadius"].get<double>();
    this->outer_radius = this->inner_radius + physics_parameters["thickness"].get<double>();
    this->hardness = physics_parameters["wallInteraction"]["hardness"].get<double>();

    this->instance.x = initial_conditions["position"]["x"].get<double>();
    this->instance.y = initial_conditions["position"]["y"].get<double>();
}

void DiskWall::compute_step(int now, double delta_time_step, ActorForce force, int *n_errors)
{ }

void DiskWall::update_state(int now)
{ }

WallInstance* DiskWall::get_instance(int time_step)
{
    return &instance;
}

double DiskWall::get_inner_radius()
{
    return this->inner_radius;
}

double DiskWall::get_hardness()
{
    return this->hardness;
}

std::string DiskWall::state_to_string(int time_step)
{
    std::stringstream strm;
    strm << "time-step: " << time_step << "\n";
    strm << "center_x: " << this->instance.x << "\n";
    strm << "center_y: " << this->instance.y << "\n";
    strm << "inner_radius: " << this->inner_radius << "\n";
    return strm.str();
}

void DiskWall::draw(int time_step, Camera *camera)
{
    double center_x = (this->instance.x - camera->x) * camera->zoom;
    double center_y = (this->instance.y - camera->y) * camera->zoom;
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