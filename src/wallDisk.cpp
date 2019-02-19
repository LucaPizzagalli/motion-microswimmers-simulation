#include "wallDisk.hpp"
#include <algorithm>
#include <sstream>

WallDisk::WallDisk(nlohmann::json physics_parameters, Map *map)
{
    this->inner_radius = physics_parameters["innerRadius"].get<double>();
    this->outer_radius = this->inner_radius + physics_parameters["thickness"].get<double>();
    this->hardness = physics_parameters["wallInteraction"]["hardness"].get<double>();
    this->coord = {
        physics_parameters["x"].get<double>(),
        physics_parameters["y"].get<double>()};
}

Vector2D WallDisk::get_coord() const
{
    return this->coord;
}

double WallDisk::get_inner_radius()
{
    return this->inner_radius;
}

double WallDisk::get_hardness()
{
    return this->hardness;
}

std::string WallDisk::state_to_string(int time_step) const
{
    std::stringstream strm;
    strm << "time-step: " << time_step << "\n";
    strm << "center_x: " << this->coord[0] << "\n";
    strm << "center_y: " << this->coord[1] << "\n";
    strm << "inner_radius: " << this->inner_radius << "\n";
    return strm.str();
}

void WallDisk::draw(int time_step, Camera *camera) const
{
    Vector2D center = (this->coord - camera->coord) * camera->zoom;
    double radius = this->outer_radius * camera->zoom;
    double middle_radius = (this->outer_radius + this->inner_radius) / 2 * camera->zoom;
    double thickness = (this->outer_radius - this->inner_radius) / 2 * camera->zoom;
    for (int x = (int)(center[0] - radius); x <= (int)(center[0] + radius) + 1; x++)
        for (int y = (int)(center[1] - radius); y <= (int)(center[1] + radius) + 1; y++)
        {
            double radius = (center - Vector2D{(double)x, (double)y}).modulus();
            double fading = std::max(1 - (radius - middle_radius) * (radius - middle_radius) / (thickness * thickness), 0.);
            camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + 255 * fading);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
}

CellForce WallDisk::interaction(Cell* cell, int now)
{
    Vector2D wallCoord = this->coord;
    CellInstance cellInstance = cell->get_instance(now);

    Vector2D coord, flagella_e, body_e;
    double distance;
    double force_body_modulus;
    double force_flagella_modulus;

    // force on body
    coord = cellInstance.coord - wallCoord;
    distance = coord.modulus();
    if (distance > 0)
    {
        body_e = coord / (-distance);
        distance = this->inner_radius - distance;

        if (distance <= 0)
            force_body_modulus = 10000.;
        else if (distance < cell->get_body_radius() * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(cell->get_body_radius(), 6.);
            double dist_6 = pow(distance, 6.);
            force_body_modulus = 24 * this->hardness * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance));
        }
        else
            force_body_modulus = 0;
    }
    else
    {
        body_e = {0., 0.};
        force_body_modulus = 0;
    }

    // force on flagella
    coord = cell->get_flagella_coord(cellInstance) - wallCoord;
    distance = coord.modulus();
    if (distance != 0)
    {
        flagella_e = coord / (-distance);
        distance = this->inner_radius - distance;
        if (distance <= 0)
            force_flagella_modulus = 10000.;
        else if (distance < cell->get_flagella_radius() * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(cell->get_flagella_radius(), 6.);
            double dist_6 = pow(distance, 6.);
            force_flagella_modulus = 24 * this->hardness * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance));
        }
        else
            force_flagella_modulus = 0;
    }
    else
    {
        flagella_e = {0., 0.};
        force_flagella_modulus = 0;
    }
    return CellForce(body_e * force_body_modulus, flagella_e * force_flagella_modulus);
}