#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>

#include "nlohmann/json.hpp"

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
    simulation_parameters["n_time_steps"] = (int)(simulation_parameters["duration"].get<double>() / simulation_parameters["time_step"].get<double>());
    simulation_parameters["n_saved_time_steps"] = (int)(simulation_parameters["duration"].get<double>() / simulation_parameters["saved_time_step"].get<double>());
    simulation_parameters["saved_time_step_size"] = std::max(1, (int)(simulation_parameters["saved_time_step"].get<double>() / simulation_parameters["time_step"].get<double>()));
    return simulation_parameters;
}

void thread_simulation(std::mutex *thread_lock, int *simulation_index, nlohmann::json physics_parameters, nlohmann::json simulation_parameters, Analyzer *analyzer, int thread_index, int *n_simulation_errors)
{
    gsl_rng_env_setup();
    gsl_rng *random_generator = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(random_generator, simulation_parameters["random_seed"].get<int>() + thread_index);
    int n_thread_simulation_errors = 0;
    bool simulate;
    do
    {
        simulate = false;
        {
            std::lock_guard<std::mutex> lock(*thread_lock);
            if (*simulation_index < simulation_parameters["n_simulations"].get<int>())
            {
                (*simulation_index)++;
                simulate = true;
                std::cout << "\tSimulation n " << *simulation_index << " (thread " << thread_index << ")...\n";
            }
        }
        if (simulate)
        {
            Simulation world(physics_parameters["parameters"], physics_parameters["initialConditions"], simulation_parameters, random_generator);
            try
            {
                n_thread_simulation_errors += world.compute_simulation();
            }
            catch (std::string error)
            {
                std::lock_guard<std::mutex> lock(*thread_lock);
                std::cout << "ERROR: " << error << "\n";
            }
            {
                std::lock_guard<std::mutex> lock(*thread_lock);
                analyzer->update_stats(&world, 0, simulation_parameters["n_time_steps"].get<int>(), simulation_parameters["saved_time_step_size"].get<int>());
                if (simulation_parameters["visualization"].get<bool>())
                {
                    std::cout << "\tVisualization...\n";
#ifdef usesdl
                    Visualization visualization;
                    visualization.render(&world, 0, simulation_parameters["n_time_steps"].get<int>(), simulation_parameters["saved_time_step_size"].get<int>());
#else
                    std::cout << "ERROR: compiled without SDL2\n";
#endif
                }
            }
        }
    } while (simulate);
    gsl_rng_free(random_generator);
    {
        std::lock_guard<std::mutex> lock(*thread_lock);
        *n_simulation_errors += n_thread_simulation_errors;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "ERROR: incorrect number of parameters\n";
        return 1;
    }
    std::stringstream strm;
    strm << "./input/" << argv[1];
    nlohmann::json physics_parameters = read_physics_parameters(strm.str().c_str());
    nlohmann::json simulation_parameters = read_simulation_parameters("./param/simulation_parameters.json");

    std::cout << "Computing simulations and probability map...\n";

    Analyzer analyzer(simulation_parameters, physics_parameters["parameters"]);

    int simulation_index = 0;
    int n_simulation_errors = 0;
    int n_threads = simulation_parameters["n_threads"].get<int>() - 1;
    std::vector<std::thread> threads;
    std::mutex thread_lock;

    for (int thread_index = 0; thread_index < n_threads; ++thread_index)
        threads.push_back(std::thread(thread_simulation, &thread_lock, &simulation_index, physics_parameters, simulation_parameters, &analyzer, thread_index, &n_simulation_errors));
    thread_simulation(&thread_lock, &simulation_index, physics_parameters, simulation_parameters, &analyzer, n_threads, &n_simulation_errors);

    for (int thread_index = 0; thread_index < n_threads; ++thread_index)
        threads[thread_index].join();

    std::cout << "Total number of simulation errors: " << n_simulation_errors << "\n";

    std::cout << "Computing stats...\n";
    analyzer.compute_stats();

    std::cout << "Saving stats...\n";
    strm.str("");
    std::string temp(argv[1]);
    strm << "output/" << temp.substr(0, temp.length() - 5);
    analyzer.save_stats(strm.str().c_str());

    return 0;
}
