#include "visualization.h"
#include <stdio.h>
#include <algorithm>
#include <sstream>
#include "simulation.h"

Visualization::Visualization()
{
	if(TTF_Init() == -1)
		printf("TTF could not initialize!\n");
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
	atexit(SDL_Quit);
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		printf("Warning: Linear texture filtering not enabled!");
	this->gWindow = SDL_CreateWindow("Browinian Simulation Visualization", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT+20, SDL_WINDOW_SHOWN);
	if (!this->gWindow)
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	this->renderer = SDL_CreateRenderer(this->gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!this->renderer)
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!this->texture)
		printf("texture could not be created! SDL Error: %s\n", SDL_GetError());
	this->font = TTF_OpenFont("font.ttf", 15);
	if(this->font == NULL )
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
	this->camera.zoom = 1.;
	this->camera.x = -SCREEN_WIDTH/2./this->camera.zoom;
	this->camera.y = -SCREEN_HEIGHT/2./this->camera.zoom;

	this->render_rectangle.x = 0;
	this->render_rectangle.y = 20;
	this->render_rectangle.w = SCREEN_WIDTH;
	this->render_rectangle.h = SCREEN_HEIGHT;
	this->text_color = {255, 255, 255};
	this->text1_rectangle.x = 0;
	this->text1_rectangle.y = 0;
	this->text2_rectangle.x = 150;
	this->text2_rectangle.y = 0;
}

void Visualization::render(Simulation *world, int start_time_step, int end_time_step, int step_size)
{
	int loop_time;
	for (int time_step=start_time_step; time_step<end_time_step; time_step+=step_size)
	{
		loop_time = SDL_GetTicks();
		SDL_RenderClear(this->renderer);

		std::stringstream strm;
		strm << "time: " << (time_step*world->get_delta_time_step());
		SDL_Surface* text_surface = TTF_RenderText_Solid(this->font, strm.str().c_str(), this->text_color);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(this->renderer, text_surface);
		this->text1_rectangle.w = text_surface->w;
		this->text1_rectangle.h = text_surface->h;
		SDL_RenderCopy(this->renderer, Message, NULL, &this->text1_rectangle);
		strm.str("");
		strm << "time step: " << time_step << "\tdt: " << step_size;
		text_surface = TTF_RenderText_Solid(this->font, strm.str().c_str(), this->text_color);
		Message = SDL_CreateTextureFromSurface(this->renderer, text_surface);
		this->text2_rectangle.w = text_surface->w;
		this->text2_rectangle.h = text_surface->h;
		SDL_RenderCopy(this->renderer, Message, NULL, &this->text2_rectangle);

		for (int x = 0; x < SCREEN_WIDTH; ++x)
			for (int y = 0; y < SCREEN_HEIGHT; ++y)
			{
				this->camera.pixels[y][x][0] = 0;
				this->camera.pixels[y][x][1] = 0;
				this->camera.pixels[y][x][2] = 0;
				this->camera.pixels[y][x][3] = 255;
			}
		world->draw_frame(time_step, &this->camera);
		SDL_UpdateTexture(this->texture, NULL, &this->camera.pixels[0][0][0], SCREEN_WIDTH*4);
        SDL_RenderCopy(this->renderer, this->texture, NULL, &this->render_rectangle);

		SDL_RenderPresent(this->renderer);

		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				return;
			else if(e.type == SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_ESCAPE)
					return;
				else if(e.key.keysym.sym == SDLK_PLUS || e.key.keysym.sym == SDLK_KP_PLUS)
				{
					camera.zoom *= 1.1;
					camera.x /= 1.1;
					camera.y /= 1.1;
				}
				else if(e.key.keysym.sym == SDLK_MINUS || e.key.keysym.sym == SDLK_KP_MINUS)
				{
					camera.zoom /= 1.1;
					camera.x *= 1.1;
					camera.y *= 1.1;
				}
				else if(e.key.keysym.sym == SDLK_o)
					step_size--;
				else if(e.key.keysym.sym == SDLK_p)
					step_size++;
			}
		}
		loop_time = SDL_GetTicks() - loop_time;
		SDL_Delay( std::max(20-loop_time, 0));
	}
}

Visualization::~Visualization()
{
    SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->gWindow);
}