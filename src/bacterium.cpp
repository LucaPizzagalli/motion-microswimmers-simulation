#include "bacterium.hpp"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>
#include <sstream>

Bacterium::Bacterium(nlohmann::json parameters, nlohmann::json initial_conditions, int total_time_steps, int step_size, gsl_rng *random_generator)
{
    int memory_size = total_time_steps / step_size;
    this->center_x = std::vector<double>(memory_size, 0);
    this->center_y = std::vector<double>(memory_size, 0);
    this->direction = std::vector<double>(memory_size, 0);
    this->tumble_countdown = std::vector<double>(memory_size, 0);
    this->tumble_speed = std::vector<double>(memory_size, 0);
    this->tumble_duration = std::vector<double>(memory_size, 0);

    this->random_generator = random_generator;
    this->step_size = step_size;

    nlohmann::json shape = parameters["shape"];
    this->body_radius = shape["body"]["radius"].get<double>();
    this->flagella_radius = shape["flagella"]["radius"].get<double>();
    this->body_flagella_distance = shape["BodyFlagellaDistance"].get<double>();
    this->rotation_center = shape["rotationCenter"].get<double>();

    this->speed = parameters["propulsion"]["speed"].get<double>();

    nlohmann::json tumble = parameters["propulsion"]["tumble"];
    this->tumble_delay_mean = tumble["delay"].get<double>();
    this->tumble_duration_mean = tumble["duration"]["mean"].get<double>();
    this->tumble_duration_std = tumble["duration"]["std"].get<double>();
    this->tumble_strength_mean = tumble["strength"]["mean"].get<double>();
    this->tumble_strength_std = tumble["strength"]["std"].get<double>();

    nlohmann::json fluid_interaction = parameters["fluidCellInteraction"];
    this->diffusivity = fluid_interaction["diffusivity"].get<double>();
    this->_sqrt_diffusivity = sqrt(this->diffusivity);
    this->persistence_time = fluid_interaction["persistenceTime"].get<double>();
    this->_sqrt_persistence_time = sqrt(this->persistence_time);
    this->shear_time = fluid_interaction["shearTime"].get<double>();

    this->next_center_x = initial_conditions["position"]["x"].get<double>();
    this->next_center_y = initial_conditions["position"]["y"].get<double>();
    this->next_direction = initial_conditions["direction"].get<double>();
    this->next_tumble_countdown = 0;
    this->next_tumble_speed = 0.;
    this->next_tumble_duration = 0.;
    this->update_state(0);
}

void Bacterium::compute_step(int now, double delta_time_step, CellForce forces)
{
    double sqrt_delta_time_step = sqrt(delta_time_step);
    double sin_direction = sin(this->prev_direction);
    double cos_direction = cos(this->prev_direction);
    double rotation = 0.;

    double torque_z = this->_compute_torque(forces, sin_direction, cos_direction);

    double torque_noise_z = gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_persistence_time;

    rotation += torque_z / this->shear_time * delta_time_step + torque_noise_z * sqrt_delta_time_step;

    double force_noise_x = gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity;
    double force_noise_y = gsl_ran_gaussian(this->random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity;

    this->next_center_x = this->prev_center_x + (cos_direction * this->speed + this->diffusivity * (forces.body_x + forces.flagella_x)) * delta_time_step + force_noise_x * sqrt_delta_time_step;
    this->next_center_y = this->prev_center_y + (sin_direction * this->speed + this->diffusivity * (forces.body_y + forces.flagella_y)) * delta_time_step + force_noise_y * sqrt_delta_time_step;

    rotation += this->_tumble(delta_time_step);
    this->_rotate(rotation, sin_direction, cos_direction);
}

void Bacterium::update_state(int now)
{
    this->prev_center_x = this->next_center_x;
    this->prev_center_y = this->next_center_y;
    this->prev_direction = this->next_direction;
    this->prev_tumble_countdown = this->next_tumble_countdown;
    this->prev_tumble_speed = this->next_tumble_speed;
    this->prev_tumble_duration = this->next_tumble_duration;

    if (now % this->step_size == 0)
    {
        int memory_slot = now / this->step_size;
        this->center_x[memory_slot] = this->prev_center_x;
        this->center_y[memory_slot] = this->prev_center_y;
        this->direction[memory_slot] = this->prev_direction;
        this->tumble_countdown[memory_slot] = this->prev_tumble_countdown;
        this->tumble_speed[memory_slot] = this->prev_tumble_speed;
        this->tumble_duration[memory_slot] = this->prev_tumble_duration;
    }
}

double Bacterium::_compute_torque(CellForce forces, double sin_direction, double cos_direction)
{
    double torque_body = -this->rotation_center * (cos_direction * forces.body_y - sin_direction * forces.body_x);
    double torque_flagella = (this->body_flagella_distance - this->rotation_center) * (cos_direction * forces.flagella_y - sin_direction * forces.flagella_x);
    return torque_body + torque_flagella;
}
double Bacterium::_tumble(double delta_time_step)
{
    if (this->tumble_strength_mean == 0.) // there is no tumble
        return 0.;

    double rotation = 0.;
    this->next_tumble_countdown = this->prev_tumble_countdown - delta_time_step;
    if (this->tumble_duration_mean == 0.) // the tumble is instantaneous
    {
        if (this->prev_tumble_countdown <= 0)
        {
            double tumble_strength = this->tumble_strength_mean + gsl_ran_gaussian(this->random_generator, this->tumble_strength_std);
            tumble_strength *= (int)gsl_rng_uniform_int(this->random_generator, 2) * 2 - 1;
            rotation = tumble_strength;
            this->next_tumble_countdown = gsl_ran_exponential(this->random_generator, this->tumble_delay_mean);
        }
    }
    else // the tumble takes its time
    {
        this->next_tumble_duration = this->prev_tumble_duration - delta_time_step;
        if (this->prev_tumble_countdown <= 0) ///// to modify
        {
            this->next_tumble_countdown = gsl_ran_exponential(this->random_generator, this->tumble_delay_mean);

            this->next_tumble_speed = this->tumble_strength_mean + gsl_ran_gaussian(this->random_generator, this->tumble_strength_std);
            this->next_tumble_speed *= (int)gsl_rng_uniform_int(this->random_generator, 2) * 2 - 1;

            this->next_tumble_duration = this->tumble_duration_mean + gsl_ran_gaussian(this->random_generator, this->tumble_duration_std);
        }
        if (this->prev_tumble_duration <= 0)
            this->next_tumble_speed = 0;

        rotation = this->next_tumble_speed * delta_time_step;
    }
    return rotation;
}

void Bacterium::_rotate(double rotation, double sin_direction, double cos_direction)
{
    if (this->rotation_center != 0.)
    {
        double x = -this->rotation_center * cos_direction;
        double y = -this->rotation_center * sin_direction;
        double cos_rotation = cos(rotation);
        double sin_rotation = sin(rotation);
        double new_x = x * cos_rotation - y * sin_rotation;
        double new_y = x * sin_rotation + y * cos_rotation;

        this->next_center_x += new_x - x;
        this->next_center_y += new_y - y;
    }
    this->next_direction += rotation;
}

double Bacterium::get_body_radius()
{
    return this->body_radius;
}
double Bacterium::get_flagella_radius()
{
    return this->flagella_radius;
}
double Bacterium::get_body_x()
{
    return this->prev_center_x;
}
double Bacterium::get_body_y()
{
    return this->prev_center_y;
}
double Bacterium::get_history_body_x(int time_step)
{
    return this->center_x[time_step / this->step_size];
}
double Bacterium::get_history_body_y(int time_step)
{
    return this->center_y[time_step / this->step_size];
}
double Bacterium::get_flagella_x()
{
    return this->prev_center_x + cos(this->prev_direction) * this->body_flagella_distance;
}
double Bacterium::get_flagella_y()
{
    return this->prev_center_y + sin(this->prev_direction) * this->body_flagella_distance;
}

std::string Bacterium::state_to_string()
{
    std::stringstream strm;
    strm << "center_x: " << this->prev_center_x << "\n";
    strm << "center_y: " << this->prev_center_y << "\n";
    strm << "direction: " << this->prev_direction << "\n";
    strm << "tumble_countdown: " << this->prev_tumble_countdown << "\n";
    strm << "tumble_speed: " << this->prev_tumble_speed << "\n";
    strm << "tumble_duration: " << this->prev_tumble_duration;
    return strm.str();
}

void Bacterium::draw(int time_step, Camera *camera)
{
    double center_x = (this->center_x[time_step / this->step_size] - camera->x) * camera->zoom;
    double center_y = (this->center_y[time_step / this->step_size] - camera->y) * camera->zoom;
    double radius = this->body_radius * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double fading = std::max(1 - ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y)) / (radius * radius), 0.);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
    center_x += cos(this->direction[time_step / this->step_size]) * this->flagella_radius * camera->zoom;
    center_y += sin(this->direction[time_step / this->step_size]) * this->flagella_radius * camera->zoom;
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