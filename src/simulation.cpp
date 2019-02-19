#include "simulation.hpp"
#include <gsl/gsl_randist.h>
#include <sstream>

#include <iostream>/////


Simulation::Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator)
    : map(physics_parameters["wallTop"]["y"].get<double>(), physics_parameters["wallBottom"]["y"].get<double>(), physics_parameters["wallLeft"]["x"].get<double>(), physics_parameters["wallRight"]["x"].get<double>(), simulation_parameters["map_cell_size"].get<double>()),
      wallDisk(physics_parameters["wallDisk"], &map),
      wallTop(physics_parameters["wallTop"], &map),
      wallBottom(physics_parameters["wallBottom"], &map),
      wallLeft(physics_parameters["wallLeft"], &map),
      wallRight(physics_parameters["wallRight"], &map)
{
    isWallDisk = physics_parameters["wallDisk"]["thickness"].get<double>() > 0;
    isWallTop = physics_parameters["wallTop"]["thickness"].get<double>() > 0;

    for (unsigned int i = 0; i < initial_conditions["cell"].size(); i++)
        this->cell.push_back(Cell(physics_parameters["cell"], initial_conditions["cell"][i], simulation_parameters, random_generator, &map));

    this->n_errors = 0;
    this->random_generator = random_generator;
    this->delta_time_step = simulation_parameters["time_step"].get<double>();
    this->n_time_steps = simulation_parameters["n_time_steps"].get<double>();
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();

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
    std::vector<CellForce> force(this->cell.size(), CellForce{{0., 0.}, {0., 0.}});
    for (unsigned int i = 0; i < this->cell.size(); i++)
    {
        std::set<Actor*> neighbours = map.check(&(this->cell[i]), this->cell[i].get_instance(this->time_step-1).coord);
        for (std::set<Actor*>::iterator it=neighbours.begin(); it!=neighbours.end(); ++it)
            force[i] += (*it)->interaction(&(this->cell[i]), this->time_step-1);
    }
    for (unsigned int i = 0; i < this->cell.size(); i++)
    {
        try
        {
            this->cell[i].compute_step(this->time_step, this->delta_time_step, force[i], &(this->n_errors));
        }
        catch (std::string error)
        {
            std::stringstream strm;
            strm << "Simulation error at time_step " << this->time_step << ": \n\t";
            strm << error << "\n";
            strm << "\nCell's previous saved state:\n"
                 << this->cell[i].state_to_string(this->time_step - 1);
            throw strm.str();
        }
    }
    for (unsigned int i = 0; i < this->cell.size(); i++)
        this->cell[i].update_state(this->time_step, &map);
}

std::vector<Cell> Simulation::get_cells() const
{
    return this->cell;
}

double Simulation::get_delta_time_step() const
{
    return this->delta_time_step;
}

void Simulation::draw_frame(int time_step, Camera *camera) const
{
    if (isWallDisk)
        wallDisk.draw(time_step, camera);
    if (isWallTop)
    {
        wallTop.draw(time_step, camera);
        wallBottom.draw(time_step, camera);
        wallLeft.draw(time_step, camera);
        wallRight.draw(time_step, camera);
    }
    for (unsigned int i = 0; i < this->cell.size(); i++)
        this->cell[i].draw(time_step, camera);
}
