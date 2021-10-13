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


using namespace std;

using InnerRadius = int;
using OuterRadius = int;
using ButtonList = std::list<Button>;


class Elbo : public Drawable {
  public:
    Elbo(
        Region _b,
        ColourScheme _cs,
        Size _s,
        InnerRadius _ir,
        OuterRadius _or
      ) :
        bounds{_b},
        colours{_cs},
        corner{_b.position, _s},
        inner_radius{_ir},
        outer_radius{_or},
        buttons{}
    {};

    Elbo(
        Region _b,
        ColourScheme _cs,
        Size _s,
        InnerRadius _ir
      ) :
        bounds{_b},
        colours{_cs},
        corner{_b.position, _s},
        inner_radius{_ir},
        outer_radius{_s.y + _ir},
        buttons{}
    {};

    Elbo(
        Region _b,
        ColourScheme _cs,
        Size _s
      ) :
        bounds{_b},
        colours{_cs},
        corner{_b.position, _s},
        outer_radius{_s.y*2},
        inner_radius{_s.y},
        buttons{}
    {};

    Region SweepRegion() const;
    Region HorizontalRegion() const;
    Region VerticalRegion() const;
    Region InnerRadiusRegion() const;
    Region OuterRadiusRegion() const;
    Region HeaderRegion() const;
    Region ContainerRegion() const;

    void SetHeader(std::string h) {
      header = h;
    }
    void AddButton();

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    Region Bounds() {
      return bounds;
    };

  protected:
    Region bounds;
    Region corner;
    ColourScheme colours;
    InnerRadius inner_radius;
    OuterRadius outer_radius;
    Gutter gutter{10, 10};
    ButtonList buttons;
    std::string header;
};
