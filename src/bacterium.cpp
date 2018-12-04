#include "bacterium.hpp"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>

Bacterium::Bacterium(nlohmann::json parameters, nlohmann::json initial_conditions, int total_time_steps)
{
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


    this->center_x = std::vector<double>(total_time_steps, 0);
    this->center_y = std::vector<double>(total_time_steps, 0);
    this->direction = std::vector<double>(total_time_steps, 0);
    this->tumble_countdown = std::vector<double>(total_time_steps, 0);
    this->tumble_speed = std::vector<double>(total_time_steps, 0);
    this->tumble_duration = std::vector<double>(total_time_steps, 0);

    this->center_x[0] = initial_conditions["position"]["x"].get<double>();
    this->center_y[0] = initial_conditions["position"]["y"].get<double>();
    this->direction[0] = initial_conditions["direction"].get<double>();
    this->tumble_countdown[0] = 0;
    this->tumble_speed[0] = 0.;
    this->tumble_duration[0] = 0.;
}

void Bacterium::compute_step(int now, double delta_time_step, CellForce forces, gsl_rng *random_generator)
{
    double sqrt_delta_time_step = sqrt(delta_time_step);
    double sin_direction = sin(direction[now - 1]);
    double cos_direction = cos(direction[now - 1]);
    double rotation = 0.;

    double torque_z = this->_compute_torque(forces, sin_direction, cos_direction);

    double torque_noise_z = gsl_ran_gaussian(random_generator, 1.) * SQRT_2 * this->_sqrt_persistence_time;

    rotation += torque_z / this->shear_time * delta_time_step + torque_noise_z * sqrt_delta_time_step;

    double force_noise_x = gsl_ran_gaussian(random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity;
    double force_noise_y = gsl_ran_gaussian(random_generator, 1.) * SQRT_2 * this->_sqrt_diffusivity;

    this->center_x[now] = this->center_x[now - 1] + (cos_direction * this->speed + this->diffusivity * (forces.body_x + forces.flagella_x)) * delta_time_step + force_noise_x * sqrt_delta_time_step;
    this->center_y[now] = this->center_y[now - 1] + (sin_direction * this->speed + this->diffusivity * (forces.body_y + forces.flagella_y)) * delta_time_step + force_noise_y * sqrt_delta_time_step;

    rotation += this->_tumble(now, delta_time_step, random_generator);
    this->_rotate(now, rotation, sin_direction, cos_direction);
}

double Bacterium::_compute_torque(CellForce forces, double sin_direction, double cos_direction)
{
    double torque_body = -this->rotation_center * (cos_direction * forces.body_y - sin_direction * forces.body_x);
    double torque_flagella = (this->body_flagella_distance - this->rotation_center) * (cos_direction * forces.flagella_y - sin_direction * forces.flagella_x);
    return torque_body + torque_flagella;
}
double Bacterium::_tumble(double now, double delta_time_step, gsl_rng *random_generator)
{
    if(this->tumble_strength_mean == 0.) // there is no tumble
        return 0.;
    
    double rotation;
    this->tumble_speed[now] = this->tumble_speed[now - 1];
    this->tumble_countdown[now] = this->tumble_countdown[now-1] - delta_time_step;
    if(this->tumble_duration_mean == 0.) // the tumble is instantaneous
    {
        if(this->tumble_countdown[now] <= 0)
        {
            double tumble_strength = this->tumble_strength_mean + gsl_ran_gaussian(random_generator, this->tumble_strength_std);
            tumble_strength *= (int)gsl_rng_uniform_int(random_generator, 2)*2-1;
            rotation = tumble_strength;
            this->tumble_countdown[now] = gsl_ran_exponential(random_generator, this->tumble_delay_mean);
        }
    }
    else // the tumble takes its time
    {
        this->tumble_duration[now] = this->tumble_duration[now - 1] - delta_time_step;
        if (this->tumble_countdown[now] <= 0)///// to modify
        {
            this->tumble_countdown[now] = gsl_ran_exponential(random_generator, this->tumble_delay_mean);

            this->tumble_speed[now] = this->tumble_strength_mean + gsl_ran_gaussian(random_generator, this->tumble_strength_std);
            this->tumble_speed[now] *= (int)gsl_rng_uniform_int(random_generator, 2) * 2 - 1;

            this->tumble_duration[now] = this->tumble_duration_mean + gsl_ran_gaussian(random_generator, this->tumble_duration_std);
        }
        if (this->tumble_duration[now] <= 0)
            this->tumble_speed[now] = 0;

        rotation = this->tumble_speed[now] * delta_time_step;
    }
    return rotation;
}

void Bacterium::_rotate(double now, double rotation, double sin_direction, double cos_direction)
{
    if(this->rotation_center != 0.)
    {
        double x = -this->rotation_center * cos_direction;
        double y = -this->rotation_center * sin_direction;
        double cos_rotation = cos(rotation);
        double sin_rotation = sin(rotation);
        double new_x = x * cos_rotation - y * sin_rotation;
        double new_y = x * sin_rotation + y * cos_rotation;

        this->center_x[now] += new_x - x;
        this->center_y[now] += new_y - y;
    }
    
    this->direction[now] = this->direction[now - 1] + rotation;
}

double Bacterium::get_body_radius(int time_step)
{
    return this->body_radius;
}
double Bacterium::get_flagella_radius(int time_step)
{
    return this->flagella_radius;
}
double Bacterium::get_body_x(int time_step)
{
    return this->center_x[time_step];
}
double Bacterium::get_body_y(int time_step)
{
    return this->center_y[time_step];
}
double Bacterium::get_flagella_x(int time_step)
{
    return this->center_x[time_step] + cos(this->direction[time_step]) * this->body_flagella_distance;
}
double Bacterium::get_flagella_y(int time_step)
{
    return this->center_y[time_step] + sin(this->direction[time_step]) * this->body_flagella_distance;
}

void Bacterium::draw(int time_step, Camera *camera)
{
    double center_x = (this->center_x[time_step] - camera->x) * camera->zoom;
    double center_y = (this->center_y[time_step] - camera->y) * camera->zoom;
    double radius = this->body_radius * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double fading = std::max(1 - ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y)) / (radius * radius), 0.);
            camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + 255 * fading);
        }
    center_x += cos(this->direction[time_step]) * this->flagella_radius * camera->zoom;
    center_y += sin(this->direction[time_step]) * this->flagella_radius * camera->zoom;
    radius = this->flagella_radius * camera->zoom;
    for (int x = (int)(center_x - radius); x <= (int)(center_x + radius) + 1; x++)
        for (int y = (int)(center_y - radius); y <= (int)(center_y + radius) + 1; y++)
        {
            double fading = std::max(1 - ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y)) / (radius * radius), 0.);
            if (this->tumble_duration[time_step] > 0)
            {
                // int color = (int)(127.5 + tumble_speed[time_step] * 50);
                // camera->pixels[y][x][1] = int(camera->pixels[y][x][1] * (1 - fading) + color * fading);
            }
            else
            {
                int color = (int)(255 * std::max(0., 1 - this->tumble_countdown[time_step] / this->tumble_delay_mean));
                camera->pixels[y][x][0] = int(camera->pixels[y][x][0] * (1 - fading) + (255 - color) * fading);
                camera->pixels[y][x][2] = int(camera->pixels[y][x][2] * (1 - fading) + color * fading);
            }
        }
}