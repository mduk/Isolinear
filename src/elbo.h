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


using namespace std;

using InnerRadius = int;
using OuterRadius = int;
using ButtonList = std::list<Button>;

class Elbo : public Drawable {
  public:
    Elbo(
        Window& w,
        Region2D b,
        Size2D s,
        std::string h
      ) :
        window{w},
        bounds{b},
        corner{b.Origin(), s},
        header{h},
        inner_radius{window.HeaderFont().Height()},
        outer_radius{window.HeaderFont().Height()},
        button_grid{
          VerticalRegion(),
          100, // row height
          Margin{0,0},
          Gutter{0,10}
        }
    {};

    Region2D SweepRegion() const;
    Region2D HorizontalRegion() const;
    Region2D VerticalRegion() const;
    Region2D InnerRadiusRegion() const;
    Region2D OuterRadiusRegion() const;
    Region2D HeaderRegion() const;
    Region2D ContainerRegion() const;

    void AddButton();

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    SDL_Rect SdlRect() const {
      return bounds.SdlRect();
    }

    Drawable* container = nullptr;

  protected:
    Window window;
    Region2D bounds;
    Region2D corner;
    ColourScheme colours{
        0xff664466, 0xffcc9999,
        0xffff9999, 0xff6666cc
      };
    InnerRadius inner_radius;
    OuterRadius outer_radius;
    Gutter gutter{10, 10};
    Grid button_grid;
    ButtonList buttons{};
    std::string header;

    bool wireframe = false;
    bool basic = false;
};
