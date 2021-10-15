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
        Region b,
        ColourScheme cs,
        Size s,
        std::string h
      ) :
        window{w},
        bounds{b},
        colours{cs},
        corner{b.position, s},
        header{h},
        inner_radius{window.HeaderFont().Height()},
        outer_radius{window.HeaderFont().Height()},
        button_grid{
          VerticalRegion(),
          RowHeight{100},
          NumCols{1},
          Margin{0,0},
          Gutter{0,10}
        }
    {};

    Region SweepRegion() const;
    Region HorizontalRegion() const;
    Region VerticalRegion() const;
    Region InnerRadiusRegion() const;
    Region OuterRadiusRegion() const;
    Region HeaderRegion() const;
    Region ContainerRegion() const;

    void AddButton();

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    SDL_Rect SdlRect() const {
      return bounds.SdlRect();
    }

    Drawable* container = nullptr;

  protected:
    Window window;
    Region bounds;
    Region corner;
    ColourScheme colours;
    InnerRadius inner_radius;
    OuterRadius outer_radius;
    Gutter gutter{10, 10};
    Grid button_grid;
    ButtonList buttons{};
    std::string header;

    bool wireframe = false;
    bool basic = false;
};
