#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "grid.h"
#include "button.h"
#include "colours.h"
#include "geometry.h"
#include "window.h"
#include "shapes.h"


using namespace std;

class NorthWestElbo : public Drawable {
  protected:
    Window& window;
    Grid& grid;

    int reach_weight{15};
    Vector2D sweep_cells{2,2};
    Vector2D gutter{10,10};
    ColourScheme colours{
        0xff664466, 0xffcc9999,
        0xffff9999, 0xff6666cc
      };
    std::string header_string{""};
    std::list<Button> buttons{};

    GridRegion SweepRegion() const {
      return grid.MultiCellRegion(
        1, 1,
        sweep_cells.x, sweep_cells.y
      );
    }

    GridRegion HorizontalRegion() const {
      return grid.MultiCellRegion(
        sweep_cells.x + 1, 1,
        grid.MaxColumns(), 2
      );
    }

    Region2D ReachRegion() const {
      GridRegion horizontal = HorizontalRegion();
      return Region2D{
          horizontal.Origin(),
          Size2D{
            horizontal.W(),
            reach_weight
          }
        };
    }

    Region2D HeaderRegion() const {
      GridRegion horizontal = HorizontalRegion();
      return Region2D{
          Position2D{
            horizontal.Origin().x,
            horizontal.Origin().y
              + reach_weight
              + gutter.y
          },
          Size2D{
            horizontal.Size().x,
            horizontal.Size().y
              - reach_weight
              - gutter.y
          }
        };
    }

    GridRegion VerticalRegion() const {
      return grid.MultiCellRegion(
        1, sweep_cells.y + 1,
        sweep_cells.x - 1, grid.MaxRows()
      );
    }

    GridRegion ContainerRegion() const {
      return grid.MultiCellRegion(
        sweep_cells.x, sweep_cells.y + 1,
        grid.MaxColumns(), grid.MaxRows()
      );
    }



  public:
    NorthWestElbo(
        Window& w,
        std::string h
      ) :
        NorthWestElbo{ w, w.grid, h }
    {};

    NorthWestElbo(
        Window& w,
        Grid& g,
        std::string h
      ) :
        window{w},
        grid{g},
        header_string{h}
    {};


    void AddButton(std::string label) {
      buttons.emplace_back(
          window,
          Region2D{},
          colours,
          label
        );
    }

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    SDL_Rect SdlRect() const {
      return grid.bounds.SdlRect();
    }
};
