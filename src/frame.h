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
    int east_frame = 3;
    int south_frame = 2;
    int west_frame = 4;

    Vector2D nesize{4,3};
    Vector2D sesize{5,4};
    Vector2D swsize{6,5};
    Vector2D nwsize{7,6};

    HorizontalButtonBar north_bar;
         NorthEastSweep northeast_sweep;
      VerticalButtonBar east_bar;
         SouthEastSweep southeast_sweep;
    HorizontalButtonBar south_bar;
         SouthWestSweep southwest_sweep;
      VerticalButtonBar west_bar;
         NorthWestSweep northwest_sweep;

  public:
    Frame(Grid g, Window& w)
      :
        grid{g},
        window{w},

        north_bar{window, grid.SubGrid(
          nwsize.x + 1, 1,
          grid.MaxColumns() - nesize.x, north_frame
        )},

        east_bar{window, grid.SubGrid(
          grid.MaxColumns() - (east_frame-1),
          nesize.y + 1,

          grid.MaxColumns(),
          grid.MaxRows() - sesize.y
        )},

        west_bar{window, grid.SubGrid(
          1,
          nwsize.y + 1,

          west_frame,
          grid.MaxRows() - swsize.y
        )},

        south_bar{window, grid.SubGrid(
          swsize.x + 1, grid.MaxRows() - (south_frame-1),
          grid.MaxColumns() - sesize.x, grid.MaxRows()
        )},

        northeast_sweep{window,
          grid.SubGrid(
              grid.MaxColumns() - (nesize.x - 1),
              1,

              grid.MaxColumns(),
              nesize.y
            ),
          Vector2D{ east_frame, north_frame },
          outer_radius,
          inner_radius
        },

        southeast_sweep{window,
          grid.SubGrid(
              grid.MaxColumns() - (sesize.x - 1),
              grid.MaxRows() - (sesize.y - 1),

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
              grid.MaxRows() - (swsize.y - 1),

              swsize.x,
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

              nwsize.x,
              nwsize.y
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
            north_bar.Draw(renderer);
      northeast_sweep.Draw(renderer);
             east_bar.Draw(renderer);
      southeast_sweep.Draw(renderer);
            south_bar.Draw(renderer);
      southwest_sweep.Draw(renderer);
             west_bar.Draw(renderer);
      northwest_sweep.Draw(renderer);
    }
};
