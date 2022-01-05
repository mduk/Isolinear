#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


using isolinear::compass;
using isolinear::geometry::vector;
using isolinear::geometry::Region2D;


extern bool drawdebug;


class Sweep : public drawable {
  protected:
    Window& window;
    Grid grid;
    vector ports;
    int outer_radius;
    int inner_radius;
    compass alignment;
    compass opposite;

  public:
    Sweep(Window& w, Grid g, vector p, int oradius, int iradius, compass ali)
      : window{w}, grid{g}, ports{p}, outer_radius{oradius}, inner_radius{iradius}, alignment{ali}
    {
      switch (alignment) {
        case compass::northeast: opposite = compass::southwest; break;
        case compass::southeast: opposite = compass::northwest; break;
        case compass::southwest: opposite = compass::northeast; break;
        case compass::northwest: opposite = compass::southeast; break;
      }
    }

    int VerticalPortSize() const {
      return ports.x;
    }

    int HorizontalPortSize() const {
      return ports.y;
    }

    virtual Region2D HorizontalPort() const = 0;
    virtual Region2D VerticalPort() const = 0;

    virtual Region2D InnerCornerRegion() const {
      return Region2D{
          HorizontalPort().Point(opposite),
          VerticalPort().Point(opposite)
        };
    }

    Region2D OuterRadiusRegion() const {
      return grid.bounds.Align(alignment, Size2D{outer_radius});
    }

    void DrawOuterRadius(SDL_Renderer* renderer) const {
      Region2D region = OuterRadiusRegion();
      region.Fill(renderer, Colours().background);
      region.QuadrantArc(renderer, alignment, Colours().frame);
    }

    Region2D Bounds() const override {
      return grid.bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      Region2D icorner = InnerCornerRegion();
      Region2D iradius = icorner.Align(alignment, Size2D{inner_radius});

      grid.bounds.Fill(renderer, Colours().frame);
      icorner.Fill(renderer, Colours().background);

      iradius.Fill(renderer, Colours().frame);
      iradius.QuadrantArc(renderer, alignment, Colours().background);
      DrawOuterRadius(renderer);

      if (drawdebug) {
        HorizontalPort().Draw(renderer);
        VerticalPort().Draw(renderer);
        icorner.Draw(renderer);
      }
    }
};

class NorthEastSweep : public Sweep {
  public:
    NorthEastSweep(Window& window, Grid grid,  vector ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, compass::northeast}
    {}

    Region2D HorizontalPort() const override {
      return grid.CalculateGridRegion(
          1, 1,
          1, ports.y
        );
    }

    Region2D VerticalPort() const override {
      return grid.CalculateGridRegion(
          grid.MaxColumns() - ports.x + 1, grid.MaxRows(),
                        grid.MaxColumns(), grid.MaxRows()
        );
    }

};

class SouthEastSweep : public Sweep {
  public:
    SouthEastSweep(Window& window, Grid grid,  vector ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, compass::southeast}
    {}

    Region2D HorizontalPort() const override {
      return grid.CalculateGridRegion(
          1, grid.MaxRows() - ports.y + 1,
          1, grid.MaxRows()
        );
    }

    Region2D VerticalPort() const override {
      return grid.CalculateGridRegion(
          grid.MaxColumns() - ports.x + 1, 1,
          grid.MaxColumns(), 1
        );
    }

};

class SouthWestSweep : public Sweep {
  public:
    SouthWestSweep(Window& window, Grid grid,  vector ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, compass::southwest}
    {}

    Region2D VerticalPort() const override {
      return grid.CalculateGridRegion(
          1, 1,
          ports.x, 1
        );
    }

    Region2D HorizontalPort() const override {
      return grid.CalculateGridRegion(
          grid.MaxColumns(), grid.MaxRows() - ports.y + 1,
                        grid.MaxColumns(), grid.MaxRows()
        );
    }

};

class NorthWestSweep : public Sweep {
  public:
    NorthWestSweep(Window& window, Grid grid,  vector ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, compass::northwest}
    {}

    Region2D HorizontalPort() const override {
      return grid.CalculateGridRegion(
          grid.MaxColumns(), 1,
          grid.MaxColumns(), ports.y
        );
    }

    Region2D VerticalPort() const override {
      return grid.CalculateGridRegion(
          1, grid.MaxRows(),
          ports.x, grid.MaxRows()
        );
    }


};
