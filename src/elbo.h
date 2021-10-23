#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

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
    Window& window;
    Grid& grid;

    int reach_weight{20};
    Vector2D sweep_cells{3,2};
    Vector2D gutter{10,10};
    ColourScheme colours{
        0xff664466, 0xffcc9999,
        0xffff9999, 0xff6666cc
      };
    std::string header_string{""};

  public:
    Elbo(
        Window& w,
        std::string h
      ) :
        window{w},
        grid{w.grid},
        header_string{h}
    {};

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    SDL_Rect SdlRect() const {
      return grid.bounds.SdlRect();
    }
};
