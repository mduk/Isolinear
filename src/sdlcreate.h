#pragma once

#include <SDL2/SDL.h>

SDL_Window* createWindow(int w, int h);
SDL_Renderer* createRenderer(SDL_Window* window);

