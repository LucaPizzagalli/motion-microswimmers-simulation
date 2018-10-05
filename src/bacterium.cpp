#include "bacterium.h"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>

Bacterium::Bacterium(int total_time_steps, double x_position, double y_position, double theta)
{
    this->body_radius = 2.5; //5 micrometer
    this->flagella_radius = 5.; //2.5 micrometer
    this->x_position = std::vector<double>(total_time_steps, 0);
    this->y_position = std::vector<double>(total_time_steps, 0);
    this->theta = std::vector<double>(total_time_steps, 0);
    this->x_position[0] = x_position;
    this->y_position[0] = y_position;
    this->theta[0] = theta;
}

void Bacterium::compute_step(int now, double delta_time_step, gsl_rng *random_generator)
{
    double k = gsl_ran_gaussian(random_generator, 1.);
    double vel_x, vel_y;
    
    // euler scheme
    // d_e/d_t = (Tw/tau_w + epsilon) cross_product e
    //double sqrt_dt = sqrt(delta_time_step);
    this->theta[now] = this->theta[now-1] + 0.2*k*delta_time_step;
    vel_x = cos(this->theta[now]);
    vel_y = sin(this->theta[now]);
    this->x_position[now] = this->x_position[now-1] + vel_x*delta_time_step;
    this->y_position[now] = this->y_position[now-1] + vel_y*delta_time_step;
}

void Bacterium::draw(int time_step, Camera *camera)
{
    double center_x = (this->x_position[time_step] - camera->x) * camera->zoom;
    double center_y = (this->y_position[time_step] - camera->y) * camera->zoom;
    double radius =  this->body_radius * camera->zoom;
    for(int x=(int)(center_x-radius); x<=(int)(center_x+radius)+1; x++)
        for(int y=(int)(center_y-radius); y<=(int)(center_y+radius)+1; y++)
        {
            double fading = std::max(1-((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y))/(radius*radius), 0.);
		    camera->pixels[y][x][1] = int(camera->pixels[y][x][1]*(1-fading)+255*fading);
        }
    center_x += cos(this->theta[time_step])*this->flagella_radius * camera->zoom;
    center_y += sin(this->theta[time_step])*this->flagella_radius * camera->zoom;
    radius =  this->flagella_radius * camera->zoom;
    for(int x=(int)(center_x-radius); x<=(int)(center_x+radius)+1; x++)
        for(int y=(int)(center_y-radius); y<=(int)(center_y+radius)+1; y++)
        {
            double fading = std::max(1-((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y))/(radius*radius), 0.);
		    camera->pixels[y][x][0] = int(camera->pixels[y][x][0]*(1-fading)+255*fading);
        }
}