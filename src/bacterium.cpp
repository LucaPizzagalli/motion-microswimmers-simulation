#include "bacterium.h"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>

Bacterium::Bacterium(int total_time_steps, double center_x, double center_y, double theta)
{
    this->body_radius = 2.5; //5 micrometer
    this->flagella_radius = 5.; //2.5 micrometer
    this->speed = 300.; //100 micrometer/second
    this->tumble_mean_time = 2.; // 2 seconds = tau_p/(k_b*T)
    this->tumble_mean_strength = M_PI_2; //pi/2 radiants
    this->tumble_std_strength = 0.1; //0.1 radiants
    this->center_x = std::vector<double>(total_time_steps, 0);
    this->center_y = std::vector<double>(total_time_steps, 0);
    this->direction_x = std::vector<double>(total_time_steps, 0);
    this->direction_y = std::vector<double>(total_time_steps, 0);
    this->theta = std::vector<double>(total_time_steps, 0);
    this->tumble_countdown = std::vector<double>(total_time_steps, 0);

    this->center_x[0] = center_x;
    this->center_y[0] = center_y;
    this->theta[0] = theta;
    this->direction_x[0] = cos(this->theta[0]);
    this->direction_y[0] = sin(this->theta[0]);
    this->tumble_countdown[0] = 0;
}

void Bacterium::compute_step(int now, double delta_time_step, Force force, gsl_rng *random_generator)
{
    double sqrt_delta_time_step = sqrt(delta_time_step);

    double torque_z = this->flagella_radius*(this->direction_x[now-1]*force.y-this->direction_y[now-1]*force.x);
    double torque_noise_z = gsl_ran_gaussian(random_generator, 1/2.);
    
    this->direction_x[now] = this->direction_x[now-1] - torque_z/4.*this->direction_y[now-1]*sqrt_delta_time_step - torque_noise_z*this->direction_y[now-1]*delta_time_step;
    this->direction_y[now] = this->direction_y[now-1] + torque_z/4.*this->direction_x[now-1]*sqrt_delta_time_step + torque_noise_z*this->direction_x[now-1]*delta_time_step;
    printf("-------------------%f\n",this->direction_y[now-1]*this->direction_y[now-1]+this->direction_x[now-1]*this->direction_x[now-1]);

    double force_noise_x = gsl_ran_gaussian(random_generator, 22.361);
    double force_noise_y = gsl_ran_gaussian(random_generator, 22.361);
    
    this->center_x[now] = this->center_x[now-1] + (direction_x[now-1]*this->speed + 250.*force.x)*delta_time_step + direction_x[now-1]*force_noise_x*sqrt_delta_time_step;
    this->center_y[now] = this->center_y[now-1] + (direction_y[now-1]*this->speed + 250.*force.y)*delta_time_step + direction_y[now-1]*force_noise_y*sqrt_delta_time_step;

    // tumble
    this->tumble_countdown[now] = this->tumble_countdown[now-1] - delta_time_step;
    if(this->tumble_countdown[now] <= 0)
    {
        double tumble_strength = this->tumble_mean_strength + gsl_ran_gaussian(random_generator, this->tumble_std_strength);
        tumble_strength *= (int)gsl_rng_uniform_int(random_generator, 2)*2-1;
        double cos_tumble = cos(tumble_strength);
        double sin_tumble = sin(tumble_strength);
        this->direction_x[now] = cos_tumble*this->direction_x[now-1] - sin_tumble*this->direction_y[now-1];
        this->direction_y[now] = sin_tumble*this->direction_x[now-1] + cos_tumble*this->direction_y[now-1];
        this->tumble_countdown[now] = gsl_ran_exponential(random_generator, this->tumble_mean_time);
    }
}

double Bacterium::getBodyRadius(int time_step)
{
    return this->body_radius;
}
double Bacterium::getFlagellaRadius(int time_step)
{
    return this->flagella_radius;
}
double Bacterium::getBodyX(int time_step)
{
    return this->center_x[time_step];
}
double Bacterium::getBodyY(int time_step)
{
    return this->center_y[time_step];
}
double Bacterium::getFlagellaX(int time_step)
{
    return this->center_x[time_step] + this->direction_x[time_step]*this->flagella_radius;
}
double Bacterium::getFlagellaY(int time_step)
{
    return this->center_y[time_step] + this->direction_y[time_step]*this->flagella_radius;
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
    center_x += this->direction_x[time_step]*this->flagella_radius * camera->zoom;
    center_y += this->direction_y[time_step]*this->flagella_radius * camera->zoom;
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