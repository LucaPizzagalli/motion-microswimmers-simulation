#include "bacterium.h"
#include <gsl/gsl_randist.h>
#include <cmath>
#include <algorithm>

Bacterium::Bacterium(int total_time_steps, double center_x, double center_y, double theta)
{
    this->body_radius = 2.5; //5 micrometer
    this->flagella_radius = 5.; //2.5 micrometer
    this->speed = 100.; //100 micrometer/second
    this->tumble_mean_time = 2.; // 2 seconds = tau_p/(k_b*T)
    this->tumble_mean_strength = M_PI_2; //pi/2 radiants
    this->tumble_std_strength = 0.1; //0.1 radiants
    this->center_x = std::vector<double>(total_time_steps, 0);
    this->center_y = std::vector<double>(total_time_steps, 0);
    this->theta = std::vector<double>(total_time_steps, 0);
    this->tumble_countdown = std::vector<double>(total_time_steps, 0);

    this->center_x[0] = center_x;
    this->center_y[0] = center_y;
    this->theta[0] = theta;
    this->tumble_countdown[0] = 0;
}

void Bacterium::compute_step(int now, double delta_time_step, Force force, gsl_rng *random_generator)
{
    double k = gsl_ran_gaussian(random_generator, 1.);
    double vel_x, vel_y;
    
    // euler scheme
    // T ~ 24+273 // kelvin degrees
    // k_b*T*mu = 250 micrometers^2 / seconds
    // nu_x = nu_y is a Gaussian white noise with zero mean and <nu_x(t)nu_x(t')> = 2*k_b*T*mu*delta(t−t')

    // d_r/d_t = speed*e + mu*F_tot + nu
    // d_center_x/d_t = speed * e_x + mu*F_tot_x + nu_x
    


    // d_e/d_t = (Tw/tau_w + epsilon) cross_product e
    //double sqrt_dt = sqrt(delta_time_step);
    this->theta[now] = this->theta[now-1] + 7.2*k*delta_time_step;
    vel_x = cos(this->theta[now]) * this->speed;
    vel_y = sin(this->theta[now]) * this->speed;
    printf("%f %f\n",force.x,force.y);
    this->center_x[now] = this->center_x[now-1] + (vel_x+force.x)*delta_time_step;
    this->center_y[now] = this->center_y[now-1] + (vel_y+force.y)*delta_time_step;

    // tumble
    this->tumble_countdown[now] = this->tumble_countdown[now-1] - delta_time_step;
    if(this->tumble_countdown[now] <= 0)
    {
        double tumble_strength = this->tumble_mean_strength + gsl_ran_gaussian(random_generator, this->tumble_std_strength);
        tumble_strength *= (int)gsl_rng_uniform_int(random_generator, 2)*2-1;
        this->theta[now] += tumble_strength;
        this->tumble_countdown[now] = gsl_ran_exponential(random_generator, this->tumble_mean_time);
        printf("--%f %f \n", this->tumble_countdown[now], tumble_strength);
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
    return this->center_x[time_step] + cos(this->theta[time_step])*this->flagella_radius;
}
double Bacterium::getFlagellaY(int time_step)
{
    return this->center_x[time_step] + sin(this->theta[time_step])*this->flagella_radius;
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
    center_x += cos(this->theta[time_step])*this->flagella_radius * camera->zoom;
    center_y += sin(this->theta[time_step])*this->flagella_radius * camera->zoom;
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