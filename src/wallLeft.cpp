#include "wallLeft.hpp"
#include <algorithm>
#include <sstream>

WallLeft::WallLeft(nlohmann::json physics_parameters, Map *map)
{
    this->x = physics_parameters["x"].get<double>();
    this->x2 = this->x - physics_parameters["thickness"].get<double>();
    this->hardness = physics_parameters["wallInteraction"]["hardness"].get<double>();
    map->vertical(this, this->x);
}

double WallLeft::get_x() const
{
    return this->x;
}

double WallLeft::get_hardness() const
{
    return this->hardness;
}

std::string WallLeft::state_to_string(int time_step) const
{
    std::stringstream strm;
    strm << "time-step: " << time_step << "\n";
    strm << "x: " << this->x << "\n";
    return strm.str();
}

void WallLeft::draw(int time_step, Camera *camera) const
{
    double middle_x = ((this->x + this->x2) / 2 - camera->coord[0]) * camera->zoom;
    double thickness = (this->x - this->x2) / 2 * camera->zoom;
    for (int x = (int)(middle_x - thickness); x <= (int)(middle_x + thickness) + 1; x++)
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            double fading = std::max(1 - (x - middle_x) * (x - middle_x) / (thickness * thickness), 0.);
            camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + 255 * fading);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
}

CellForce WallLeft::interaction(Cell *cell, int now)
{
    CellInstance cellInstance = cell->get_instance(now);

    double distance;
    double force_body_modulus;
    double force_flagella_modulus;

    // force on body
    distance = cellInstance.coord[0] - this->x;

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

    // force on flagella
    distance = cell->get_flagella_coord(cellInstance)[0] - this->x;
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

    return CellForce(Vector2D{force_body_modulus, 0.}, Vector2D{force_flagella_modulus, 0.});
}