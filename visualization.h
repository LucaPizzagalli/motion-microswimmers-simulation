#ifndef VISUALIZATION_H
#define VISUALIZATION_H
#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 500

#include <SDL2/SDL.h>
#include "simulation.h"

class Visualization
{
    SDL_Window *gWindow;
	SDL_Renderer *gRenderer;
	SDL_Texture *texture;
	unsigned char screen_color[SCREEN_HEIGHT][SCREEN_WIDTH][4];

public:
    Visualization();
    void render(Simulation world, int start_time_step, int end_time_step);
    ~Visualization();
};

#endif
