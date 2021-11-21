#pragma once

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "miso.h"

#include "window.h"
#include "drawable.h"
#include "grid.h"
#include "geometry.h"
#include "pointerevent.h"


using std::cout;


class HorizontalProgressBar : public Drawable {
  protected:
    Grid grid;
    unsigned max;
    unsigned val;
    unsigned gutter = 6;

    bool draw_stripes = false;
    bool draw_tail = false;

    Region2D bar_region;
    Size2D segment_size;
    unsigned remainder_px;

  public:
    unsigned filled_segments = 0;
    unsigned n_segments = 0;

    HorizontalProgressBar(Grid _g)
      : grid{_g}
      , bar_region(
          Position2D(
            grid.bounds.NearX() + (gutter * 2),
            grid.bounds.NearY() + (gutter * 2)
          ),
          Position2D(
            grid.bounds.FarX() - (gutter * 2),
            grid.bounds.FarY() - (gutter * 2)
          )
        )
      , segment_size{ (int) gutter, bar_region.H() }
      , n_segments{ (unsigned) (bar_region.W() / segment_size.x) }
      , remainder_px{ bar_region.W() % segment_size.x }
    { };

    unsigned Max() {
      return max;
    }

    void Max(unsigned m) {
      if (m > val) {
        val = m;
      }

      max = m;
    }

    unsigned Val() {
      return val;
    }

    void Val(unsigned v) {
      if (v > max) {
        val = max;
      }
      else {
        val = v;
      }
    }

    void Inc(unsigned v) {
      if (val + v > max) {
        val = max;
      }
      else {
        val = val + v;
      }
    }

    void Dec(unsigned v) {
      if (v > val) {
        val = 0;
      }
      else {
        val = val - v;
      }
    }

    void DrawTail(bool v) {
      draw_tail = v;
    }

    void DrawStripes(bool v) {
      draw_stripes = v;
    }

    Region2D Bounds() const override {
      return grid.bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      boxColor(renderer,
          grid.bounds.NearX(), grid.bounds.NearY(),
          grid.bounds.FarX(),  grid.bounds.FarY(),
          Colours().frame
        );
      boxColor(renderer,
          grid.bounds.NearX() + gutter, grid.bounds.NearY() + gutter,
          grid.bounds.FarX() - gutter,  grid.bounds.FarY() - gutter,
          Colours().background
        );



      if (draw_stripes) for (int i=0; i<n_segments; i++) {
        Region2D region{
            Position2D{ bar_region.Near().x + (segment_size.x * i), bar_region.Near().y },
            segment_size
          };

        if (i % 2 == 0) {
          region.Fill(renderer, 0xff666666);
        }
        else {
          region.Fill(renderer, 0xff333333);
        }
      }

      unsigned   segment_range = max / n_segments;

      unsigned filled_segments = val / segment_range;
      unsigned      subsegment = val % segment_range;

      if (filled_segments == n_segments) {
        Region2D region(
            Position2D( bar_region.Near().x + (segment_size.x * filled_segments), bar_region.Near().y ),
            Size2D( segment_size.x + remainder_px, segment_size.y )
          );
        region.Fill(renderer, Colours().white);
      }
      else {
        Region2D region{
            Position2D{ bar_region.Near().x + (segment_size.x * filled_segments), bar_region.Near().y },
            segment_size
          };
        region.Fill(renderer, Colours().frame);
      }

      if (draw_tail) for (int i=0; i<filled_segments; i++) {
        Region2D region{
            Position2D{ bar_region.Near().x + (segment_size.x * i), bar_region.Near().y },
            segment_size
          };
        region.Fill(renderer, Colours().frame);
      }

    }
};
