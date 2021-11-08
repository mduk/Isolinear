#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

extern bool drawdebug;

class Sweep : public Drawable {
  protected:
    Window& window;
    Grid grid;
    Vector2D ports;
    int outer_radius;
    int inner_radius;
    Compass alignment;
    Compass opposite;

  public:
    Sweep(Window& w, Grid g, Vector2D p, int oradius, int iradius, Compass ali)
      : window{w}, grid{g}, ports{p}, outer_radius{oradius}, inner_radius{iradius}, alignment{ali}
    {
      switch (alignment) {
        case Compass::NORTHEAST: opposite = Compass::SOUTHWEST; break;
        case Compass::SOUTHEAST: opposite = Compass::NORTHWEST; break;
        case Compass::SOUTHWEST: opposite = Compass::NORTHEAST; break;
        case Compass::NORTHWEST: opposite = Compass::SOUTHEAST; break;
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
    NorthEastSweep(Window& window, Grid grid,  Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, Compass::NORTHEAST}
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
    SouthEastSweep(Window& window, Grid grid,  Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, Compass::SOUTHEAST}
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
    SouthWestSweep(Window& window, Grid grid,  Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, Compass::SOUTHWEST}
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
    NorthWestSweep(Window& window, Grid grid,  Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, ports, oradius, iradius, Compass::NORTHWEST}
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
