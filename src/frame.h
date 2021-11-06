#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "drawable.h"
#include "pointerevent.h"
#include "grid.h"
#include "window.h"
#include "buttonbar.h"
#include "sweep.h"

extern bool drawdebug;

class Frame : public Drawable {
  protected:
    Grid grid;
    Window& window;

    int outer_radius{90};
    int inner_radius{50};

    int north_frame = 2;
    int east_frame  = 2;
    int south_frame = 2;
    int west_frame  = 2;

    Vector2D northeast_sweep_size{3,3};
    Vector2D southeast_sweep_size{3,3};
    Vector2D southwest_sweep_size{3,3};
    Vector2D northwest_sweep_size{3,3};

    HorizontalButtonBar north_bar;
         NorthEastSweep northeast_sweep;
      VerticalButtonBar east_bar;
         SouthEastSweep southeast_sweep;
    HorizontalButtonBar south_bar;
         SouthWestSweep southwest_sweep;
      VerticalButtonBar west_bar;
         NorthWestSweep northwest_sweep;

  public:
    Frame(Grid g, Window& w, int nf, int ef, int sf, int wf)
      :
        grid{g},
        window{w},
        north_frame{nf},
        east_frame{ef},
        south_frame{sf},
        west_frame{wf},

        north_bar{window, grid.SubGrid(
          northwest_sweep_size.x + 1, 1,
          grid.MaxColumns() - northeast_sweep_size.x, north_frame
        )},

        east_bar{window, grid.SubGrid(
          grid.MaxColumns() - (east_frame-1),
          northeast_sweep_size.y + 1,

          grid.MaxColumns(),
          grid.MaxRows() - southeast_sweep_size.y
        )},

        west_bar{window, grid.SubGrid(
          1,
          northwest_sweep_size.y + 1,

          west_frame,
          grid.MaxRows() - southwest_sweep_size.y
        )},

        south_bar{window, grid.SubGrid(
          southwest_sweep_size.x + 1, grid.MaxRows() - (south_frame-1),
          grid.MaxColumns() - southeast_sweep_size.x, grid.MaxRows()
        )},

        northeast_sweep{window,
          grid.SubGrid(
              grid.MaxColumns() - (northeast_sweep_size.x - 1),
              1,

              grid.MaxColumns(),
              northeast_sweep_size.y
            ),
          Vector2D{ east_frame, north_frame },
          outer_radius,
          inner_radius
        },

        southeast_sweep{window,
          grid.SubGrid(
              grid.MaxColumns() - (southeast_sweep_size.x - 1),
              grid.MaxRows() - (southeast_sweep_size.y - 1),

              grid.MaxColumns(),
              grid.MaxRows()
            ),
          Vector2D{ east_frame, south_frame },
          outer_radius,
          inner_radius
        },

        southwest_sweep{window,
          grid.SubGrid(
              1,
              grid.MaxRows() - (southwest_sweep_size.y - 1),

              southwest_sweep_size.x,
              grid.MaxRows()
            ),
          Vector2D{ west_frame, south_frame },
          outer_radius,
          inner_radius
        },

        northwest_sweep{window,
          grid.SubGrid(
              1,
              1,

              northwest_sweep_size.x,
              northwest_sweep_size.y
            ),
          Vector2D{ west_frame, north_frame },
          outer_radius,
          inner_radius
        }
    {
      north_bar.AddButton("92-5668");
      east_bar.AddButton("92-5668");
      west_bar.AddButton("92-5668");
      south_bar.AddButton("516.375");
    }

    Region2D Bounds() const override {
      return grid.bounds;
    }

    virtual ColourScheme Colours() const override {
      return Drawable::Colours();
    }

    virtual void Colours(ColourScheme cs) override {
      Drawable::Colours(cs);
            north_bar.Colours(cs);
      northeast_sweep.Colours(cs);
             east_bar.Colours(cs);
      southeast_sweep.Colours(cs);
            south_bar.Colours(cs);
      southwest_sweep.Colours(cs);
             west_bar.Colours(cs);
      northwest_sweep.Colours(cs);
    }

    void Draw(SDL_Renderer* renderer) const override {

      if (north_frame > 0) north_bar.Draw(renderer);
      if ( east_frame > 0)  east_bar.Draw(renderer);
      if (south_frame > 0) south_bar.Draw(renderer);
      if ( west_frame > 0)  west_bar.Draw(renderer);


      northeast_sweep.Draw(renderer);
      southeast_sweep.Draw(renderer);
      southwest_sweep.Draw(renderer);
      northwest_sweep.Draw(renderer);
    }
};
