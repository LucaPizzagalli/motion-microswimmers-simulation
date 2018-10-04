#include "bacterium.h"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>

Bacterium::Bacterium(int total_time_steps, double x_position, double y_position, double theta)
{
    this->body_radius = 5;
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

void Bacterium::draw(int time_step, unsigned char screen_color[SCREEN_HEIGHT][SCREEN_WIDTH][4])
{
    double center_x = this->y_position[time_step];
    double center_y = this->x_position[time_step];
    for(int x=(int)(center_x-this->body_radius); x<=(int)(center_x+this->body_radius)+1; x++)
        for(int y=(int)(center_y-this->body_radius); y<=(int)(center_y+this->body_radius)+1; y++)
        {
            double fading = std::max(1-((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y))/(this->body_radius*this->body_radius), 0.);
		    screen_color[x][y][1] = int(255*fading);
        }
}