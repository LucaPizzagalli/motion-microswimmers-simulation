#include "simulation.hpp"
#include <cmath>
#include <gsl/gsl_randist.h>
#include <sstream>

Simulation::Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator)
    : disk_wall(physics_parameters["wall"], initial_conditions["wall"]),
      bacterium(physics_parameters["cell"], initial_conditions["cell"], simulation_parameters, random_generator)
{
    this->random_generator = random_generator;
    this->delta_time_step = simulation_parameters["time_step"].get<double>();
    this->n_time_steps = simulation_parameters["n_time_steps"].get<double>();
    this->step_size = simulation_parameters["saved_time_step_size"].get<int>();
    this->time_step = 0;
}

void Simulation::compute_simulation()
{
    for (; this->time_step < this->n_time_steps - 1; ++this->time_step)
        this->compute_next_step();
}

void Simulation::compute_next_step()
{
    try
    {
        CellForce forces = this->disk_wall.force_acting_on(&(this->bacterium));
        this->bacterium.compute_step(this->time_step, this->delta_time_step, forces);
    }
    catch (std::string error)
    {
        std::stringstream strm;
        strm << "Simulation error at time_step " << this->time_step << ": \n\t";
        strm << error << "\n";
        strm << "Cell's state:\n"
             << this->bacterium.state_to_string()
             << "Cell's previous saved state:\n"
             << this->bacterium.state_to_string(this->time_step - this->step_size);
        throw strm.str();
    }

    this->bacterium.update_state(this->time_step);
}

Bacterium *Simulation::get_bacterium()
{
    return &(this->bacterium);
}

double Simulation::get_delta_time_step()
{
    return this->delta_time_step;
}

void Simulation::draw_frame(int time_step, Camera *camera)
{
    this->disk_wall.draw(time_step, camera);
    this->bacterium.draw(time_step, camera);
}