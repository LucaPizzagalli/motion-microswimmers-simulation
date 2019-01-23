#include "simulation.hpp"
#include <cmath>
#include <gsl/gsl_randist.h>
#include <sstream>

Simulation::Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator)
{
    this->n_errors = 0;
    this->random_generator = random_generator;
    this->delta_time_step = simulation_parameters["time_step"].get<double>();
    this->n_time_steps = simulation_parameters["n_time_steps"].get<double>();
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();
    this->actor.push_back(std::make_shared<DiskWall>(physics_parameters["wall"], initial_conditions["wall"], simulation_parameters));
    this->actor.push_back(std::make_shared<Cell>(physics_parameters["cell"], initial_conditions["cell"], simulation_parameters, random_generator));
    this->time_step = 1;
}

int Simulation::compute_simulation()
{
    for (; this->time_step < this->n_time_steps; ++this->time_step)
        this->compute_next_step();
    return this->n_errors;
}

void Simulation::compute_next_step()
{
    std::vector<ActorForce> force(this->actor.size(), ActorForce{0., 0., 0., 0.});
    for (unsigned int i = 0; i < this->actor.size(); i++)
        for (unsigned int j = i + 1; j < this->actor.size(); j++)
        {
            ForceCouple couple = this->interaction(this->actor[i], this->actor[j]);
            force[i] += couple.a;
            force[j] += couple.b;
        }
    for (unsigned int i = 0; i < this->actor.size(); i++)
    {
        try
        {
            this->actor[i]->compute_step(this->time_step, this->delta_time_step, force[i], &(this->n_errors));
        }
        catch (std::string error)
        {
            std::stringstream strm;
            strm << "Simulation error at time_step " << this->time_step << ": \n\t";
            strm << error << "\n";
            strm << "\nCell's previous saved state:\n"
                 << this->actor[i]->state_to_string(this->time_step - 1);
            throw strm.str();
        }
    }
    for (unsigned int i = 0; i < this->actor.size(); i++)
        this->actor[i]->update_state(this->time_step);
}

ForceCouple Simulation::interaction(std::shared_ptr<Actor> actor1, std::shared_ptr<Actor> actor2)
{
    DiskWall *diskWall1 = dynamic_cast<DiskWall *>(actor1.get());
    Cell *cell1 = dynamic_cast<Cell *>(actor1.get());
    Cell *cell2 = dynamic_cast<Cell *>(actor2.get());

    if (diskWall1 && cell2)
    {
        WallInstance *wallInstance = diskWall1->get_instance(this->time_step - 1);
        CellInstance *cellInstance = cell2->get_instance(this->time_step - 1);

        double x, y, distance;
        double body_e_x, body_e_y, force_body_modulus;
        double flagella_e_x, flagella_e_y, force_flagella_modulus;

        // force on body
        x = cellInstance->x - wallInstance->x;
        y = cellInstance->y - wallInstance->y;
        distance = sqrt(x * x + y * y);
        if (distance > 0)
        {
            body_e_x = -x / distance;
            body_e_y = -y / distance;
            distance = diskWall1->get_inner_radius() - distance;

            if (distance <= 0)
                force_body_modulus = 10000.;
            else if (distance < cell2->get_body_radius() * 1.122462) // 2^(1/6)
            {
                double rad_6 = pow(cell2->get_body_radius(), 6.);
                double dist_6 = pow(distance, 6.);
                force_body_modulus = 24 * diskWall1->get_hardness() * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance));
            }
            else
                force_body_modulus = 0;
        }
        else
        {
            body_e_x = 0;
            body_e_y = 0;
            force_body_modulus = 0;
        }

        // force on flagella
        x = cell2->get_flagella_x(cellInstance) - wallInstance->x;
        y = cell2->get_flagella_y(cellInstance) - wallInstance->y;

        distance = sqrt(x * x + y * y);
        if (distance != 0)
        {
            flagella_e_x = -x / distance;
            flagella_e_y = -y / distance;
            distance = diskWall1->get_inner_radius() - distance;
            if (distance <= 0)
                force_flagella_modulus =10000.;
            else if (distance < cell2->get_flagella_radius() * 1.122462) // 2^(1/6)
            {
                double rad_6 = pow(cell2->get_flagella_radius(), 6.);
                double dist_6 = pow(distance, 6.);
                force_flagella_modulus = 24 * diskWall1->get_hardness() * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance));
            }
            else
                force_flagella_modulus = 0;
        }
        else
        {
            flagella_e_x = 0;
            flagella_e_y = 0;
            force_flagella_modulus = 0;printf("Hey solo una volta"); ////
        }
        return {ActorForce(0., 0., 0., 0.), ActorForce(force_body_modulus * body_e_x, force_body_modulus * body_e_y, force_flagella_modulus * flagella_e_x, force_flagella_modulus * flagella_e_y)};
    }
    else if (cell1 && cell2)
    {
        return {ActorForce(0., 0., 0., 0.), ActorForce(0., 0., 0., 0.)};
    }
    else
        printf("ehy qualcosa non va"); ////
    throw "aa";
}

std::vector<std::shared_ptr<Actor>> Simulation::get_actors()
{
    return this->actor;
}

double Simulation::get_delta_time_step()
{
    return this->delta_time_step;
}

void Simulation::draw_frame(int time_step, Camera *camera)
{
    for (unsigned int i = 0; i < this->actor.size(); i++)
        this->actor[i]->draw(time_step, camera);
}
