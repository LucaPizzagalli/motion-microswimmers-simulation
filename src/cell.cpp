#include "cell.hpp"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>
#include <sstream>

Cell::Cell(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator)
{
    this->throw_errors = simulation_parameters["throw_errors"];
    int memory_size = simulation_parameters["n_saved_time_steps"].get<int>();
    this->instance = std::vector<CellInstance>(memory_size, CellInstance(0., 0., 0., 0., 0., 0.));
    this->tumble_countdown = std::vector<double>(memory_size, 0);
    this->tumble_speed = std::vector<double>(memory_size, 0);
    this->tumble_duration = std::vector<double>(memory_size, 0);

    this->random_generator = random_generator;
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();

    nlohmann::json shape = physics_parameters["shape"];
    this->body_radius = shape["body"]["radius"].get<double>();
    this->flagella_radius = shape["flagella"]["radius"].get<double>();
    this->body_flagella_distance = shape["BodyFlagellaDistance"].get<double>();
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

    this->next_instance.x = initial_conditions["position"]["x"].get<double>();
    this->next_instance.y = initial_conditions["position"]["y"].get<double>();
    this->next_instance.direction = initial_conditions["direction"].get<double>();
    this->next_instance.tumble_countdown = 0;
    this->next_instance.tumble_speed = 0.;
    this->next_instance.tumble_duration = 0.;
    this->update_state(0);
}

void Cell::compute_step(int now, double delta_time_step, ActorForce force, int *n_errors)
{
    double sqrt_delta_time_step = sqrt(delta_time_step);
    double sin_direction = sin(this->prev_instance.direction);
    double cos_direction = cos(this->prev_instance.direction);
    double rotation = 0.;

    double delta_x_force = this->diffusivity * (force.x + force.x2) * delta_time_step;
    double delta_y_force = this->diffusivity * (force.y + force.y2) * delta_time_step;
    double delta_x_y_force = delta_x_force * delta_x_force + delta_y_force * delta_y_force;
    if (delta_x_y_force > 10.)
    {
        (*n_errors)++;
        printf("over delta_f: %f\n", delta_x_y_force);
        delta_x_force /= sqrt(delta_x_y_force);
        delta_y_force /= sqrt(delta_x_y_force);
        if (this->throw_errors)
        {
            std::stringstream strm;
            strm << "Force on cell too strong";
            strm << "pos x" << this->prev_instance.x;
            strm << "pos y" << this->prev_instance.y;
            throw strm.str();
        }
    }

    double force_noise_x = 0;//gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity * this->_sqrt_noise_force_strength;
    double force_noise_y = 0;//gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity * this->_sqrt_noise_force_strength;

    this->next_instance.x = this->prev_instance.x + cos_direction * this->speed * delta_time_step + delta_x_force + force_noise_x * sqrt_delta_time_step;
    this->next_instance.y = this->prev_instance.y + sin_direction * this->speed * delta_time_step + delta_y_force + force_noise_y * sqrt_delta_time_step;

    double torque_z = this->_compute_torque(force, sin_direction, cos_direction);

    double torque_noise_z = gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_noise_torque_strength;

    rotation += torque_z / this->shear_time * delta_time_step + torque_noise_z * sqrt_delta_time_step;

    rotation += this->_tumble(delta_time_step);
    this->_rotate(rotation, sin_direction, cos_direction);
}

void Cell::update_state(int now)
{
    this->prev_instance = this->next_instance;
    this->instance[now / this->step_size] = this->prev_instance;
}

double Cell::_compute_torque(ActorForce force, double sin_direction, double cos_direction)
{
    double torque_body = -this->rotation_center * (cos_direction * force.y - sin_direction * force.x);
    double torque_flagella = (this->body_flagella_distance - this->rotation_center) * (cos_direction * force.y2 - sin_direction * force.x2);
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
            double tumble_strength = this->tumble_strength_mean + gsl_ran_gaussian(this->random_generator, this->tumble_strength_std);
            tumble_strength *= (int)gsl_rng_uniform_int(this->random_generator, 2) * 2 - 1;
            rotation = tumble_strength;
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
        if (this->prev_instance.tumble_duration <= 0)
            this->next_instance.tumble_speed = 0;

        rotation = this->next_instance.tumble_speed * delta_time_step;
    }
    return rotation;
}

void Cell::_rotate(double rotation, double sin_direction, double cos_direction)
{
    if (this->rotation_center != 0.)
    {
        double x = -this->rotation_center * cos_direction;
        double y = -this->rotation_center * sin_direction;
        double cos_rotation = cos(rotation);
        double sin_rotation = sin(rotation);
        double new_x = x * cos_rotation - y * sin_rotation;
        double new_y = x * sin_rotation + y * cos_rotation;

        this->next_instance.x += new_x - x;
        this->next_instance.y += new_y - y;
    }
    this->next_instance.direction += rotation;
}

double Cell::get_body_radius()
{
    return this->body_radius;
}
double Cell::get_flagella_radius()
{
    return this->flagella_radius;
}
double Cell::get_flagella_x(CellInstance *instance)
{
    return instance->x + cos(instance->direction) * this->body_flagella_distance;
}
double Cell::get_flagella_y(CellInstance *instance)
{
    return instance->y + sin(instance->direction) * this->body_flagella_distance;
}
CellInstance* Cell::get_instance(int time_step)
{
    return &(this->instance[time_step / this->step_size]);
}
std::string Cell::state_to_string(int time_step)
{
    std::stringstream strm;
    int memory_slot = time_step / this->step_size;
    strm << "time-step: " << time_step << "\n";
    strm << "center_x: " << this->instance[memory_slot].x << "\n";
    strm << "center_y: " << this->instance[memory_slot].y << "\n";
    strm << "direction: " << this->instance[memory_slot].direction << "\n";
    strm << "tumble_countdown: " << this->tumble_countdown[memory_slot] << "\n";
    strm << "tumble_speed: " << this->tumble_speed[memory_slot] << "\n";
    strm << "tumble_duration: " << this->tumble_duration[memory_slot] << "\n";
    return strm.str();
}

void Cell::draw(int time_step, Camera *camera)
{
    CellInstance* instance = this->get_instance(time_step);
    double center_x = (instance->x - camera->x) * camera->zoom;
    double center_y = (instance->y - camera->y) * camera->zoom;
    double radius = this->body_radius * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double fading = std::max(1 - ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y)) / (radius * radius), 0.);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
    center_x = (this->get_flagella_x(instance) - camera->x) * camera->zoom;
    center_y = (this->get_flagella_y(instance) - camera->y) * camera->zoom;
    radius = this->flagella_radius * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double fading = std::max(1 - ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y)) / (radius * radius), 0.);
            if (this->tumble_duration[time_step / this->step_size] > 0)
            {
                // int color = (int)(127.5 + tumble_speed[time_step/this->step_size] * 50);/////
                // camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + color * fading);
            }
            else
            {
                int color = (int)(255 * std::max(0., 1 - this->tumble_countdown[time_step / this->step_size] / this->tumble_delay_mean));
                camera->pixels[y][x][0] = int(camera->pixels[y][x][0] * (1 - fading) + (255 - color) * fading);
                camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + color * fading);
            }
        }
}