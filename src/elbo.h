#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "drawable.h"
#include "grid.h"
#include "button.h"
#include "colours.h"
#include "geometry.h"
#include "window.h"
#include "shapes.h"


using namespace std;

class Elbo : public Drawable {
  protected:
    Grid& grid;
    int reach_weight{30};
    Coordinate gutter{10,10};
    ColourScheme colours{
        0xaa664466, 0xaacc9999,
        0xaaff9999, 0xaa6666cc
      };

  public:
    Elbo(Grid& g) : grid{g} {};

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    SDL_Rect SdlRect() const {
      return grid.bounds.SdlRect();
    }
};
