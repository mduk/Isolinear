#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Sweep : public Region2D {
  protected:
    Window& window;
    Grid grid;

  public:
    Sweep(Window& w, Grid g)
      : window{w}, grid{g}
    {
      printf("Sweep Grid: %d,%d\n", grid.MaxColumns(), grid.MaxRows());
    }

    void Draw(SDL_Renderer* renderer) const override {
      grid.bounds.Fill(renderer, 0xffffffff);
      grid.Column(1).bounds.QuadrantArc(renderer, Compass::NORTHWEST, 0xff0000ff);
      grid.Row(1).bounds.Fill(renderer, 0xff0000ff);
    }
};
