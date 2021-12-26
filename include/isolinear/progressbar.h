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


namespace isolinear::ui::progress {

  class horizontal_bar : public drawable {
    protected:
      Grid grid;
      unsigned max = 100;
      unsigned val = 50;
      unsigned gutter = 6;

      bool draw_stripes = true;
      bool draw_tail = true;

      Colour bar_colour;
      Region2D bar_region;
      Size2D segment_size;
      unsigned remainder_px = 0;
      unsigned n_segments = 0;


    public:
      miso::signal<> signal_valuechanged;

      horizontal_bar(Grid _g)
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
          Val(m);
        }

        max = m;
      }

      unsigned Val() {
        return val;
      }

      void Val(unsigned v) {
        if (v == val) {
          return;
        }

        if (v > max) {
          val = max;
        }
        else {
          val = v;
        }

        emit signal_valuechanged();
      }

      void Inc(unsigned v) {
        if (val + v > max) {
          Val(max);
        }
        else {
          Val(val + v);
        }
      }

      void Dec(unsigned v) {
        if (v > val) {
          Val(0);
        }
        else {
          Val(val - v);
        }
      }

      unsigned Segments() const {
        return n_segments;
      }

      unsigned FilledSegments() const {
        return (val * n_segments) / max;
      }

      bool DrawTail() const {
        return draw_tail;
      }
      void DrawTail(bool v) {
        draw_tail = v;
      }

      bool DrawStripes() const {
        return draw_stripes;
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

        if (draw_stripes) {
          for (int i=0; i<n_segments; i++) {
            Region2D region{
                Position2D{ bar_region.Near().x + (segment_size.x * i), bar_region.Near().y },
                segment_size
              };

            if (i % 2 == 0) {
              region.Fill(renderer, Colours().background);
            }
            else {
              region.Fill(renderer, Colours().light_alternate);
            }
          }
        }

        Colour bar_colour = Colours().active;

        Region2D filled_region{
            Position2D{ bar_region.Near().x + (segment_size.x * FilledSegments()), bar_region.Near().y },
            segment_size
          };
        filled_region.Fill(renderer, bar_colour);

        if (draw_tail) {
          for (int i=0; i<FilledSegments(); i++) {
            Region2D region{
                Position2D{ bar_region.Near().x + (segment_size.x * i), bar_region.Near().y },
                segment_size
              };
            region.Fill(renderer, bar_colour);
          }
        }
      }
  };

}
