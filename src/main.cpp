#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "include/json.hpp"

#include "simulation.hpp"
#include "analyzer.hpp"
#include "visualization.hpp"

nlohmann::json read_physics_parameters(std::string filename)
{
    std::ifstream input_file(filename);
    nlohmann::json physics_parameters;
    input_file >> physics_parameters;
    input_file.close();
    return physics_parameters;
}

nlohmann::json read_simulation_parameters(std::string filename)
{
    std::ifstream input_file(filename);
    nlohmann::json simulation_parameters;
    input_file >> simulation_parameters;
    input_file.close();
    return simulation_parameters;
}

int main(int argc, char *argv[])
{
    nlohmann::json physics_parameters = read_physics_parameters("./param/physics_parameters.json");
    nlohmann::json simulation_parameters = read_physics_parameters("./param/simulation_parameters.json");

    double delta_time_step = simulation_parameters["time_step"].get<double>();
    int n_time_steps = (int)(simulation_parameters["duration"].get<double>() / delta_time_step);
    int step_size = std::max(1, (int)(simulation_parameters["saved_time_step"].get<double>() / simulation_parameters["time_step"].get<double>()));
    double map_margin = physics_parameters["parameters"]["wall"]["innerRadius"].get<double>();

    gsl_rng_env_setup();
    gsl_rng *random_generator = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(random_generator, simulation_parameters["random_seed"].get<int>());

    std::cout << "Computing simulations and probability map...\n";
    Analyzer analyzer(-map_margin, -map_margin, map_margin, map_margin, simulation_parameters["probability_map_width"].get<int>(), simulation_parameters["probability_map_height"].get<int>());
    for (int simulation_index = 0; simulation_index < simulation_parameters["n_simulations"].get<int>(); ++simulation_index)
    {
        std::cout << "\tSimulation n " << simulation_index + 1 << "...\n";

        Simulation world(physics_parameters["parameters"], physics_parameters["initialConditions"], delta_time_step, n_time_steps, step_size, random_generator);
        try
        {
            for (int i = 0; i < n_time_steps - 1; ++i)
                world.compute_next_step();
            analyzer.update_probability_map(&world, 0, n_time_steps, step_size);
        }
        catch (std::string error)
        {
            std::cout << "ERROR: " << error << "\n";
        }
        if (simulation_parameters["visualization"].get<bool>())
        {
            std::cout << "Visualization...\n";
            Visualization visualization;
            visualization.render(&world, 0, n_time_steps, step_size);
        }
    }

    std::cout << "Computing radial probability...\n";
    analyzer.compute_radial_probability(physics_parameters["parameters"]["wall"]["innerRadius"].get<double>(), 0., 0.);

    std::cout << "Computing near-wall probability...  ";
    double near_wall = analyzer.compute_near_wall_probability(physics_parameters["parameters"]["wall"]["innerRadius"].get<double>());
    std::cout << near_wall << "\n";

    std::cout << "Saving stuff...\n";
    std::stringstream strm;
    strm << "output/r_boh_probability_map.csv";
    analyzer.save_probability_map(strm.str().c_str());
    strm.str("");
    strm << "output/r_boh_radial_probability.csv";
    analyzer.save_radial_probability(strm.str().c_str());

    gsl_rng_free(random_generator);
    return 0;
}
