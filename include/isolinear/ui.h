#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "grid.h"
#include "colours.h"
#include "pointerevent.h"

namespace isolinear::ui {

  // Widgets

  class horizontal_rule : public drawable {

    protected:
      Window& window;
      Grid grid;

    public:
      horizontal_rule(Window& w, Grid g)
        : window(w)
        , grid(g)
      {}

    public:
      Region2D Bounds() const {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const {
        auto bound_height = grid.bounds.H();
        auto offset_px = (bound_height - grid.gutter.y) / 2;

        auto hrule_near_x = grid.bounds.NearX();
        auto hrule_near_y = grid.bounds.NearY() + offset_px;

        auto hrule_far_x = grid.bounds.FarX();
        auto hrule_far_y = grid.bounds.FarY() - offset_px;

        Region2D hrule(
            Position2D(hrule_near_x, hrule_near_y),
            Position2D(hrule_far_x,  hrule_far_y)
          );

        hrule.Fill(renderer, Colours().frame);
      }
  };

}
