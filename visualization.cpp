#include "visualization.h"
#include <SDL2/SDL.h>

Visualization::Visualization()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
	atexit(SDL_Quit);
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		printf("Warning: Linear texture filtering not enabled!");
	this->gWindow = SDL_CreateWindow("SDL stuff", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (this->gWindow == nullptr)
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	this->gRenderer = SDL_CreateRenderer(*gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (this->gRenderer == nullptr)
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
	this->texture = SDL_CreateTexture(*gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture == nullptr)
		printf("texture could not be created! SDL Error: %s\n", SDL_GetError());
	for (int x = 0; x < SCREEN_WIDTH; x++)
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			screen_color[y][x][0] = 0;
			screen_color[y][x][1] = 0;
			screen_color[y][x][2] = 0;
			screen_color[y][x][3] = 255;
		}
}
Visualization::~Visualization()
{
    SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
}