#include "cell.hpp"
#include <gsl/gsl_randist.h>
#include <algorithm>
#include <sstream>

Cell::Cell(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator, Map *map)
{
    this->throw_errors = simulation_parameters["throw_errors"];
    int memory_size = simulation_parameters["n_saved_time_steps"].get<int>();
    this->instance = std::vector<CellInstance>(memory_size, CellInstance({0., 0.}, 0., 0., 0., 0.));

    this->random_generator = random_generator;
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();

    nlohmann::json shape = physics_parameters["shape"];
    this->body_radius = shape["body"]["radius"].get<double>();
    this->flagella_radius = shape["flagella"]["radius"].get<double>();
    this->body_flagella_distance = std::max(this->body_radius, this->flagella_radius);
    this->rotation_center = shape["rotationCenter"].get<double>();

    this->speed = physics_parameters["propulsion"]["speed"].get<double>();

    nlohmann::json tumble = physics_parameters["propulsion"]["tumble"];
    this->tumble_delay_mean = tumble["delay"].get<double>();
    this->tumble_duration_mean = tumble["duration"]["mean"].get<double>();
    this->tumble_duration_std = tumble["duration"]["std"].get<double>();
    this->tumble_strength_mean = tumble["strength"]["mean"].get<double>();
    this->tumble_strength_std = tumble["strength"]["std"].get<double>();

    nlohmann::json fluid_interaction = physics_parameters["fluidCellInteraction"];
    this->diffusivity = fluid_interaction["diffusivity"].get<double>();
    this->_sqrt_diffusivity = sqrt(this->diffusivity);
    this->shear_time = fluid_interaction["shearTime"].get<double>();

    nlohmann::json noise = physics_parameters["noise"];
    this->_sqrt_noise_force_strength = sqrt(noise["force"]["strength"].get<double>());
    this->_sqrt_noise_torque_strength = sqrt(noise["torque"]["strength"].get<double>());

    this->next_instance.coord = {
        initial_conditions["position"]["x"].get<double>(),
        initial_conditions["position"]["y"].get<double>()};
    this->next_instance.direction = initial_conditions["direction"].get<double>();
    this->next_instance.tumble_countdown = 0;
    this->next_instance.tumble_speed = 0.;
    this->next_instance.tumble_duration = 0.;
    this->prev_instance = this->next_instance;
    this->instance[0] = this->prev_instance;
}

void Cell::compute_step(int now, double delta_time_step, CellForce force, int *n_errors)
{
    double sqrt_delta_time_step = sqrt(delta_time_step);
    Vector2D e_direction = {cos(this->prev_instance.direction), sin(this->prev_instance.direction)};
    double rotation = 0.;

    Vector2D pos_force = (force.body + force.flagella) * this->diffusivity * delta_time_step;

    if (pos_force.square() > 4.)
    {
        (*n_errors)++;
        pos_force /= pos_force.modulus() / 2.;
        if (this->throw_errors)
        {
            std::stringstream strm;
            strm << "Force on cell too strong";
            strm << "pos x" << this->prev_instance.coord[0];
            strm << "pos y" << this->prev_instance.coord[1];
            throw strm.str();
        }
    }

    Vector2D force_noise = {
        gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity * this->_sqrt_noise_force_strength,
        gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity * this->_sqrt_noise_force_strength};

    this->next_instance.coord = this->prev_instance.coord + e_direction * this->speed * delta_time_step + pos_force + force_noise * sqrt_delta_time_step;

    double torque_z = this->_compute_torque(force, e_direction);

    double torque_noise_z = gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_noise_torque_strength;

    rotation += torque_z / this->shear_time * delta_time_step + torque_noise_z * sqrt_delta_time_step;

    rotation += this->_tumble(delta_time_step);
    this->_rotate(rotation, e_direction);
}

void Cell::update_state(int now, Map *map)
{
    map->depart(this, this->prev_instance.coord);
    this->prev_instance = this->next_instance;
    map->arrive(this, this->prev_instance.coord);
    this->instance[now / this->step_size] = this->prev_instance;
}

double Cell::_compute_torque(CellForce force, Vector2D e_direction)
{
    double torque_body = -this->rotation_center * e_direction.cross(force.body);
    double torque_flagella = (this->body_flagella_distance - this->rotation_center) * e_direction.cross(force.flagella);
    return torque_body + torque_flagella;
}
double Cell::_tumble(double delta_time_step)
{
    if (this->tumble_strength_mean == 0.) // there is no tumble
        return 0.;

    double rotation = 0.;
    this->next_instance.tumble_countdown = this->prev_instance.tumble_countdown - delta_time_step;
    if (this->tumble_duration_mean == 0.) // the tumble is instantaneous
    {
        if (this->prev_instance.tumble_countdown <= 0)
        {
            rotation = this->tumble_strength_mean + gsl_ran_gaussian(this->random_generator, this->tumble_strength_std);
            rotation *= (int)gsl_rng_uniform_int(this->random_generator, 2) * 2 - 1;
            this->next_instance.tumble_countdown = gsl_ran_exponential(this->random_generator, this->tumble_delay_mean);
        }
    }
    else // the tumble takes its time
    {
        this->next_instance.tumble_duration = this->prev_instance.tumble_duration - delta_time_step;
        if (this->prev_instance.tumble_countdown <= 0) ///// to modify
        {
            this->next_instance.tumble_countdown = gsl_ran_exponential(this->random_generator, this->tumble_delay_mean);

            this->next_instance.tumble_speed = this->tumble_strength_mean + gsl_ran_gaussian(this->random_generator, this->tumble_strength_std);
            this->next_instance.tumble_speed *= (int)gsl_rng_uniform_int(this->random_generator, 2) * 2 - 1;

            this->next_instance.tumble_duration = this->tumble_duration_mean + gsl_ran_gaussian(this->random_generator, this->tumble_duration_std);
        }
        if (this->next_instance.tumble_duration <= 0)
            this->next_instance.tumble_speed = 0;

        rotation = this->next_instance.tumble_speed * delta_time_step;
    }
    return rotation;
}

void Cell::_rotate(double rotation, Vector2D e_direction)
{
    if (this->rotation_center != 0.)
    {
        Vector2D pos = e_direction * (-this->rotation_center);
        Vector2D e_rotation_reversed = {sin(rotation), cos(rotation)};
        Vector2D new_pos = {
            pos.cross(e_rotation_reversed),
            pos * e_rotation_reversed};
        this->next_instance.coord += new_pos - pos;
    }
    this->next_instance.direction = this->prev_instance.direction + rotation;
}

CellForce Cell::interaction(Cell *cell, int now)
{
    CellInstance cellInstance1 = this->get_instance(now - 1);
    CellInstance cellInstance2 = cell->get_instance(now - 1);

    Vector2D coord1[2], coord2[2], e[4];

    coord1[0] = cellInstance1.coord;
    coord1[1] = this->get_flagella_coord(cellInstance1);
    coord2[0] = cellInstance2.coord;
    coord2[1] = cell->get_flagella_coord(cellInstance2);

    double distance;
    double force_modulus[4], size[4];

    size[0] = this->get_body_radius() + cell->get_body_radius();
    size[1] = this->get_body_radius() + cell->get_flagella_radius();
    size[2] = this->get_flagella_radius() + cell->get_body_radius();
    size[3] = this->get_flagella_radius() + cell->get_flagella_radius();

    for (int i = 0; i < 4; i++)
    {
        Vector2D coord = coord2[i % 2] - coord1[i / 2];
        distance = coord.modulus();
        e[i] = coord / distance;

        if (distance < size[i] * 1.122462) // 2^(1/6)
        {
            double rad_6 = pow(size[i], 6.);
            double dist_6 = pow(distance, 6.);
            force_modulus[i] = 24 * 10. * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance)); //cell hardness
        }
        else
            force_modulus[i] = 0;
    }
    // CellForce force1(e[0] * (-force_modulus[0]) + e[1] * (-force_modulus[1]), e[2] * (-force_modulus[2]) + e[3] * (-force_modulus[3]));
    return CellForce(e[0] * force_modulus[0] + e[2] * force_modulus[2], e[1] * force_modulus[1] + e[3] * force_modulus[3]);
}

double Cell::get_body_radius() const
{
    return this->body_radius;
}
double Cell::get_flagella_radius() const
{
    return this->flagella_radius;
}
Vector2D Cell::get_flagella_coord(CellInstance instance) const
{
    return instance.coord + Vector2D{cos(instance.direction), sin(instance.direction)} * this->body_flagella_distance;
}
CellInstance Cell::get_instance(int time_step) const
{
    return this->instance[time_step / this->step_size];
}
std::string Cell::state_to_string(int time_step) const
{
    std::stringstream strm;
    int memory_slot = time_step / this->step_size;
    strm << "time-step: " << time_step << "\n";
    strm << "center_x: " << this->instance[memory_slot].coord[0] << "\n";
    strm << "center_y: " << this->instance[memory_slot].coord[1] << "\n";
    strm << "direction: " << this->instance[memory_slot].direction << "\n";
    strm << "tumble_countdown: " << this->instance[memory_slot].tumble_countdown << "\n";
    strm << "tumble_speed: " << this->instance[memory_slot].tumble_speed << "\n";
    strm << "tumble_duration: " << this->instance[memory_slot].tumble_duration << "\n";
    return strm.str();
}

void Cell::draw(int time_step, Camera *camera) const
{
    CellInstance instance = this->get_instance(time_step);
    Vector2D center = (instance.coord - camera->coord) * camera->zoom;
    double radius = this->body_radius * camera->zoom;
    for (int x = (int)(center[0] - radius); x <= (int)(center[0] + radius) + 1; x++)
        for (int y = (int)(center[1] - radius); y <= (int)(center[1] + radius) + 1; y++)
        {
            double fading = std::max(1 - (center - Vector2D{(double)x, (double)y}).square() / (radius * radius), 0.);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
    center = (this->get_flagella_coord(instance) - camera->coord) * camera->zoom;
    radius = this->flagella_radius * camera->zoom;
    for (int x = (int)(center[0] - radius); x <= (int)(center[0] + radius) + 1; x++)
        for (int y = (int)(center[1] - radius); y <= (int)(center[1] + radius) + 1; y++)
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                double fading = std::max(1 - (center - Vector2D{(double)x, (double)y}).square() / (radius * radius), 0.);
                if (this->instance[time_step / this->step_size].tumble_duration > 0)
                {
                    int color = (int)(127.5 + this->instance[time_step/this->step_size].tumble_speed * 50);/////
                    camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + color * fading);
                }
                else
                {
                    int color = (int)(255 * std::max(0., 1 - this->instance[time_step / this->step_size].tumble_countdown / this->tumble_delay_mean));
                    camera->pixels[y][x][0] = int(camera->pixels[y][x][0] * (1 - fading) + (255 - color) * fading);
                    camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + color * fading);
                }
            }
}