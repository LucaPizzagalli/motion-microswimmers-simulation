#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>
#include "nlohmann/json.hpp"
#include "definition.hpp"
#include "wallDisk.hpp"
#include "wallTop.hpp"
#include "wallBottom.hpp"
#include "wallLeft.hpp"
#include "wallRight.hpp"
#include "cell.hpp"
#include "map.hpp"

class Simulation
{
    int n_errors;
    gsl_rng *random_generator;

    double delta_time_step;
    int n_time_steps;
    int time_step;
    int step_size;

    Map map;

    std::vector<Cell> cell;
    bool isWallDisk;
    WallDisk wallDisk;
    bool isWallTop;
    WallTop wallTop;
    WallBottom wallBottom;
    WallLeft wallLeft;
    WallRight wallRight;


public:
    Simulation(nlohmann::json physics_parameters, nlohmann::json initial_conditions, nlohmann::json simulation_parameters, gsl_rng *random_generator);
    void compute_next_step();
    int compute_simulation();
    double get_delta_time_step() const;
    std::vector<Cell> get_cells() const;
    void draw_frame(int time_step, Camera *camera) const;
};

#endif