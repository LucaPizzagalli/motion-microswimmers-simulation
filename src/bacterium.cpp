#include "bacterium.h"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>

Bacterium::Bacterium(int total_time_steps, double center_x, double center_y, double direction)
{
    this->body_radius = 2.5; //2.5 micrometers
    this->flagella_radius = 5.; //5 micrometers
    this->speed = 100.; //100 micrometers/seconds
    this->tumble_mean_time = 2.; // 2 seconds = tau_p/(k_b*T)
    this->tumble_mean_strength = M_PI_2; //pi/2 radiants
    this->tumble_std_strength = 0.1; //0.1 radiants
    this->center_x = std::vector<double>(total_time_steps, 0);
    this->center_y = std::vector<double>(total_time_steps, 0);
    this->direction = std::vector<double>(total_time_steps, 0);
    this->tumble_countdown = std::vector<double>(total_time_steps, 0);

    this->center_x[0] = center_x;
    this->center_y[0] = center_y;
    this->direction[0] = direction;
    this->tumble_countdown[0] = 0;
}

void Bacterium::compute_step(int now, double delta_time_step, Force force, gsl_rng *random_generator)
{
    double sqrt_delta_time_step = sqrt(delta_time_step);
    double sin_direction = sin(direction[now-1]);
    double cos_direction = cos(direction[now-1]);

    double torque_z = -1*this->flagella_radius*(cos_direction*force.y - sin_direction*force.x);
    double torque_noise_z = gsl_ran_gaussian(random_generator, 1.)*1.; // sqrt(2*1/2) = 1.

    this->direction[now] = this->direction[now-1] - 3.333*torque_z*delta_time_step - torque_noise_z*sqrt_delta_time_step;

    double force_noise_x = gsl_ran_gaussian(random_generator, 1.)*22.361; // sqrt(2*k_b*T*mu) = sqrt(2*250) = 22.361
    double force_noise_y = gsl_ran_gaussian(random_generator, 1.)*22.361;

    this->center_x[now] = this->center_x[now-1] + (cos_direction*this->speed + 250.*force.x)*delta_time_step + cos_direction*force_noise_x*sqrt_delta_time_step;
    this->center_y[now] = this->center_y[now-1] + (sin_direction*this->speed + 250.*force.y)*delta_time_step + sin_direction*force_noise_y*sqrt_delta_time_step;


    // tumble
    // this->tumble_countdown[now] = this->tumble_countdown[now-1] - delta_time_step;
    // if(this->tumble_countdown[now] <= 0)
    // {
    //     double tumble_strength = this->tumble_mean_strength + gsl_ran_gaussian(random_generator, this->tumble_std_strength);
    //     tumble_strength *= (int)gsl_rng_uniform_int(random_generator, 2)*2-1;
    //     this->direction[now] += tumble_strength;
    //     this->tumble_countdown[now] = gsl_ran_exponential(random_generator, this->tumble_mean_time);
    // }
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
    return this->center_x[time_step] + cos(this->direction[time_step])*this->flagella_radius;
}
double Bacterium::get_flagella_y(int time_step)
{
    return this->center_y[time_step] + sin(this->direction[time_step])*this->flagella_radius;
}
std::vector<double> Bacterium::get_history_body_x(int start_time_step, int end_time_step)
{
    std::vector<double>::const_iterator first = this->center_x.begin() + start_time_step;
    std::vector<double>::const_iterator last = this->center_x.begin() + end_time_step;
    std::vector<double> newVec(first, last);
    return newVec;
}
std::vector<double> Bacterium::get_history_body_y(int start_time_step, int end_time_step)
{
    std::vector<double>::const_iterator first = this->center_y.begin() + start_time_step;
    std::vector<double>::const_iterator last = this->center_y.begin() + end_time_step;
    std::vector<double> newVec(first, last);
    return newVec;
}

void Bacterium::draw(int time_step, Camera *camera)
{
    double center_x = (this->center_x[time_step] - camera->x) * camera->zoom;
    double center_y = (this->center_y[time_step] - camera->y) * camera->zoom;
    double radius =  this->body_radius * camera->zoom;
    for(int x=(int)(center_x-radius); x<=(int)(center_x+radius)+1; x++)
        for(int y=(int)(center_y-radius); y<=(int)(center_y+radius)+1; y++)
        {
            double fading = std::max(1-((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y))/(radius*radius), 0.);
		    camera->pixels[y][x][1] = int(camera->pixels[y][x][1]*(1-fading)+255*fading);
        }
    center_x += cos(this->direction[time_step])*this->flagella_radius * camera->zoom;
    center_y += sin(this->direction[time_step])*this->flagella_radius * camera->zoom;
    radius =  this->flagella_radius * camera->zoom;
    for(int x=(int)(center_x-radius); x<=(int)(center_x+radius)+1; x++)
        for(int y=(int)(center_y-radius); y<=(int)(center_y+radius)+1; y++)
        {
            int color = (int)(255*std::max(0., 1-this->tumble_countdown[time_step]));
            double fading = std::max(1-((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y))/(radius*radius), 0.);
		    camera->pixels[y][x][0] = int(camera->pixels[y][x][0]*(1-fading)+255*fading);
            camera->pixels[y][x][2] = int(camera->pixels[y][x][2]*(1-fading)+color*fading);
        }
}