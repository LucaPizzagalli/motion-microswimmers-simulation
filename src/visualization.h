#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "definition.h"
#include "simulation.h"

class Visualization
{
    SDL_Window *gWindow;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
    TTF_Font* font;
    SDL_Color text_color;
    SDL_Rect render_rectangle;
    SDL_Rect text1_rectangle;
    SDL_Rect text2_rectangle;
    Camera camera;

public:
    Visualization();
    void render(Simulation *world, int start_time_step, int end_time_step, int step_size);
    ~Visualization();
};

#endif
