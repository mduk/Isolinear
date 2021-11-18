#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "miso.h"

#include "window.h"
#include "drawable.h"
#include "grid.h"
#include "geometry.h"
#include "pointerevent.h"


class HorizontalProgressBar : public Drawable {
  protected:
    Grid grid;
    unsigned max;
    unsigned val;

  public:
    HorizontalProgressBar(Grid _g, unsigned _m, unsigned _v)
      : grid{_g}, max{_m}, val{_v} {}

    void Max(unsigned m) {
      max = m;
    }

    void Val(unsigned v) {
      val = v;
    }

    virtual Region2D Bounds() const override {
      return grid.bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      int g = 5;

      boxColor(renderer,
          grid.bounds.NearX(), grid.bounds.NearY(),
          grid.bounds.FarX(),  grid.bounds.FarY(),
          Colours().frame
        );
      boxColor(renderer,
          grid.bounds.NearX() + g, grid.bounds.NearY() + g,
          grid.bounds.FarX() - g,  grid.bounds.FarY() - g,
          Colours().background
        );

      int g2 = g * 2;

      Position2D bar_near{
          grid.bounds.NearX() + g2,
          grid.bounds.NearY() + g2,
        };

      Position2D bar_far{
          grid.bounds.FarX() - g2,
          grid.bounds.FarY() - g2,
        };

      Size2D bar_size{
          bar_far.x - bar_near.x,
          bar_far.y - bar_near.y
        };

      unsigned bar_length = bar_size.x;
      unsigned bar_segments = 256;

      unsigned segment_width = bar_length / bar_segments;
      unsigned segment_range =        max / bar_segments;

      unsigned segments   = val / segment_range;
      unsigned subsegment = val % segment_range;

      bar_size.x = segment_width * segments;

      Region2D bar_region{bar_near, bar_size};

      boxColor(renderer,
          bar_region.NearX(), bar_region.NearY(),
          bar_region.FarX(), bar_region.FarY(),
          Colours().light_alternate
        );

    }
};


