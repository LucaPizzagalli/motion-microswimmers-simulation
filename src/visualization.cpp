#include "visualization.h"
#include <stdio.h>
#include <algorithm>
#include "simulation.h"

Visualization::Visualization()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
	atexit(SDL_Quit);
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		printf("Warning: Linear texture filtering not enabled!");
	this->gWindow = SDL_CreateWindow("Browinian Simulation Visualization", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!this->gWindow)
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	this->gRenderer = SDL_CreateRenderer(this->gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!this->gRenderer)
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
	this->texture = SDL_CreateTexture(this->gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!this->texture)
		printf("texture could not be created! SDL Error: %s\n", SDL_GetError());
	for (int x = 0; x < SCREEN_WIDTH; ++x)
		for (int y = 0; y < SCREEN_HEIGHT;++y)
		{
			screen_color[y][x][0] = 0;
			screen_color[y][x][1] = 0;
			screen_color[y][x][2] = 0;
			screen_color[y][x][3] = 255;
		}
}

void Visualization::render(Simulation world, int start_time_step, int end_time_step)
{
	int loop_time;
	for (int time_step=start_time_step; time_step<end_time_step; time_step++)
	{
		loop_time = SDL_GetTicks();

		for (int x = 0; x < SCREEN_WIDTH; ++x)
			for (int y = 0; y < SCREEN_HEIGHT; ++y)
			{
				screen_color[y][x][0] = 0;
				screen_color[y][x][1] = 0;
				screen_color[y][x][2] = 0;
				screen_color[y][x][3] = 255;
			}
		world.draw_frame(time_step, this->screen_color);
		SDL_UpdateTexture(texture, NULL, &screen_color[0][0][0], SCREEN_WIDTH*4);
        SDL_RenderCopy(this->gRenderer, texture, NULL, NULL);
		SDL_RenderPresent(this->gRenderer);

		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				return;
			else if(e.type == SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_ESCAPE)
					return;
			}
		}
		loop_time = SDL_GetTicks() - loop_time;
		SDL_Delay( std::max(5-loop_time, 0));
	}
}

Visualization::~Visualization()
{
    SDL_DestroyRenderer(this->gRenderer);
	SDL_DestroyWindow(this->gWindow);
}