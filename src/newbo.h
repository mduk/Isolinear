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


class Newbo : public Drawable {
  public:
    Newbo(Window& w) : window{w} {};

    void Draw(SDL_Renderer* renderer) const {
      Region sweep = window.grid.MultiCellRegion(1,1,  3, 2);
      sweep.Fill(renderer, 0xffcc8866);

      sweep.AlignNorthWest(Size{sweep.size.y})
           .Fill(renderer, 0xff000000);

      Region* horizontal = new Region{window.grid.MultiCellRegion(4,1, 12,2)};

      int reach_weight = 30;

      // Reach from horizontal
      Region* reach = new Region{*horizontal};
        reach->size.y = reach_weight;
        horizontal->size.y -= reach_weight;
        horizontal->position.y += reach_weight;

      Region* iradius = new Region{sweep.AlignSouthEast(Size{horizontal->size.y})};

      // Bar
      Grid button_grid = window.grid.SubGrid(1,3, 2,12);



      window.HeaderFont().RenderTextWest(renderer, *horizontal, "MAIN WINDOW TITLE");
    };

    void OnMouseButtonDown(SDL_MouseButtonEvent&) { }

    SDL_Rect SdlRect() const {
      return window.grid.bounds.SdlRect();
    }

  protected:
    Window window;
};
