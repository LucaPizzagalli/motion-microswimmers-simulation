#include "simulation.hpp"
#include <gsl/gsl_randist.h>
#include <sstream>

Simulation::Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator)
{
    this->n_errors = 0;
    this->random_generator = random_generator;
    this->delta_time_step = simulation_parameters["time_step"].get<double>();
    this->n_time_steps = simulation_parameters["n_time_steps"].get<double>();
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();

    for (unsigned int i = 0; i < initial_conditions["wall"].size(); i++)
        this->actor.push_back(std::make_shared<DiskWall>(physics_parameters["wall"], initial_conditions["wall"][i], simulation_parameters));
    for (unsigned int i = 0; i < initial_conditions["cell"].size(); i++)
        this->actor.push_back(std::make_shared<Cell>(physics_parameters["cell"], initial_conditions["cell"][i], simulation_parameters, random_generator));
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
    std::vector<ActorForce> force(this->actor.size(), ActorForce{{0., 0.}, {0., 0.}});
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

ForceCouple Simulation::interaction(std::shared_ptr<Actor> actor1, std::shared_ptr<Actor> actor2) const
{
    DiskWall *diskWall1 = dynamic_cast<DiskWall *>(actor1.get());
    Cell *cell1 = dynamic_cast<Cell *>(actor1.get());
    Cell *cell2 = dynamic_cast<Cell *>(actor2.get());

    if (diskWall1 && cell2)
    {
        WallInstance wallInstance1 = diskWall1->get_instance(this->time_step - 1);
        CellInstance cellInstance2 = cell2->get_instance(this->time_step - 1);
        Vector2D coord1[1], coord2[2], e[2];
        double distance, force_modulus[2], size[2];

        coord1[0] = wallInstance1.coord;
        coord2[0] = cellInstance2.coord;
        coord2[1] = cell2->get_flagella_coord(cellInstance2);


        size[0] = cell2->get_body_radius();
        size[1] = cell2->get_flagella_radius();

        for (int i = 0; i < 2; i++)
        {
            Vector2D coord = coord2[i % 2] - coord1[0];
            distance = coord.modulus();
            if (distance > 0)
            {
                e[i] = coord / (-distance);
                distance = diskWall1->get_inner_radius() - distance;
                if (distance <= 0)
                    force_modulus[i] = 10000.;
                else if (distance < size[i] * 1.122462) // 2^(1/6)
                {
                    double rad_6 = pow(size[i], 6.);
                    double dist_6 = pow(distance, 6.);
                    force_modulus[i] = 24 * diskWall1->get_hardness() * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance));
                }
                else
                    force_modulus[i] = 0;
            }
            else
            {
                e[i] = {0., 0.};
                force_modulus[i] = 0;
            }
        }
        ActorForce force1;
        ActorForce force2(e[0] * force_modulus[0], e[1] * force_modulus[1]);
        return {force1, force2};
    }
    else if (cell1 && cell2)
    {
        CellInstance cellInstance1 = cell1->get_instance(this->time_step - 1);
        CellInstance cellInstance2 = cell2->get_instance(this->time_step - 1);
        Vector2D coord1[2], coord2[2], e[4];
        double distance, force_modulus[4], size[4];

        coord1[0] = cellInstance1.coord;
        coord1[1] = cell1->get_flagella_coord(cellInstance1);
        coord2[0] = cellInstance2.coord;
        coord2[1] = cell2->get_flagella_coord(cellInstance2);

        size[0] = cell1->get_body_radius() + cell2->get_body_radius();
        size[1] = cell1->get_body_radius() + cell2->get_flagella_radius();
        size[2] = cell1->get_flagella_radius() + cell2->get_body_radius();
        size[3] = cell1->get_flagella_radius() + cell2->get_flagella_radius();

        for (int i = 0; i < 4; i++)
        {
            Vector2D coord = coord2[i % 2] - coord1[i / 2];
            distance = coord.modulus();
            e[i] = coord / distance;
            if (distance < size[i] * 1.122462) // 2^(1/6)
            {
                double rad_6 = pow(size[i], 6.);
                double dist_6 = pow(distance, 6.);
                force_modulus[i] = 24 * 10. /* cell hardness */ * (2 * rad_6 * rad_6 / (dist_6 * dist_6 * distance) - rad_6 / (dist_6 * distance)); /////
            }
            else
                force_modulus[i] = 0;
        }
        ActorForce force1(e[0] * (-force_modulus[0]) + e[1] * (-force_modulus[1]), e[2] * (-force_modulus[2]) + e[3] * (-force_modulus[3]));
        ActorForce force2(e[0] * force_modulus[0] + e[2] * force_modulus[2], e[1] * force_modulus[1] + e[3] * force_modulus[3]);
        return {force1, force2};
    }
    else
        throw "Interaction not implemented";
}

std::vector<std::shared_ptr<Actor>> Simulation::get_actors() const
{
    return this->actor;
}

double Simulation::get_delta_time_step() const
{
    return this->delta_time_step;
}

void Simulation::draw_frame(int time_step, Camera *camera) const
{
    for (unsigned int i = 0; i < this->actor.size(); i++)
        this->actor[i]->draw(time_step, camera);
}
