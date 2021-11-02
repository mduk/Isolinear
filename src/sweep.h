#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Sweep : public Drawable {
  protected:
    Window& window;
    Grid grid;
    Vector2D size;
    Vector2D ports;
    int outer_radius;
    int inner_radius;

  public:
    Sweep(Window& w, Grid g, Vector2D s, Vector2D p, int oradius, int iradius)
      : window{w}, grid{g}, size{s}, ports{p}, outer_radius{oradius}, inner_radius{iradius}
    {}

    virtual SDL_Rect SdlRect() const {
      return grid.bounds.SdlRect();
    }

    virtual void Draw(SDL_Renderer*) const = 0;
};

class NorthEastSweep : public Sweep {
  public:
    NorthEastSweep(Window& window, Grid grid, Vector2D size, Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, size, ports, oradius, iradius}
    {}

    void Draw(SDL_Renderer* renderer) const override {
      Compass alignment{NORTHEAST};
      Compass opposite{SOUTHWEST};

      Region2D hport = grid.CalculateGridRegion(1, 1, 1, ports.y);
      Region2D vport = grid.CalculateGridRegion(size.x - ports.x+1, size.y, size.x, size.y);

      Position2D hportn = hport.NorthWest();
      Position2D hports = hport.SouthWest();
      Position2D vportw = vport.SouthWest();
      Position2D vporte = vport.SouthEast();

      Region2D ocorner = grid.bounds.Align(alignment, Size2D{outer_radius});

      Region2D icorner = grid.bounds.Align(opposite, Size2D{
          vportw.x - hports.x,
          vportw.y - hports.y
        });

      Region2D iradius = icorner.Align(alignment, Size2D{inner_radius});

      grid.bounds.Fill(renderer, Colours().frame);

      ocorner.Fill(renderer, Colours().background);
      ocorner.QuadrantArc(renderer, alignment, Colours().frame);

      icorner.Fill(renderer, Colours().background);
      iradius.Fill(renderer, Colours().frame);
      iradius.QuadrantArc(renderer, alignment, Colours().background);
    }
};

class SouthEastSweep : public Sweep {
  public:
    SouthEastSweep(Window& window, Grid grid, Vector2D size, Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, size, ports, oradius, iradius}
    {}

    void Draw(SDL_Renderer* renderer) const override {
      Compass alignment{SOUTHEAST};
      Compass opposite{NORTHWEST};

      Region2D hport = grid.CalculateGridRegion(1, 1, 1, ports.y);
      Region2D vport = grid.CalculateGridRegion(size.x - ports.x+1, size.y, size.x, size.y);

      Position2D hportn = hport.NorthWest();
      Position2D hports = hport.SouthWest();
      Position2D vportw = vport.SouthWest();
      Position2D vporte = vport.SouthEast();

      Region2D ocorner = grid.bounds.Align(alignment, Size2D{outer_radius});

      Region2D icorner = grid.bounds.Align(opposite, Size2D{
          vportw.x - hports.x,
          vportw.y - hports.y
        });

      Region2D iradius = icorner.Align(alignment, Size2D{inner_radius});

      grid.bounds.Fill(renderer, Colours().frame);

      ocorner.Fill(renderer, Colours().background);
      ocorner.QuadrantArc(renderer, alignment, Colours().frame);

      icorner.Fill(renderer, Colours().background);
      iradius.Fill(renderer, Colours().frame);
      iradius.QuadrantArc(renderer, alignment, Colours().background);
    }
};

class SouthWestSweep : public Sweep {
  public:
    SouthWestSweep(Window& window, Grid grid, Vector2D size, Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, size, ports, oradius, iradius}
    {}

    void Draw(SDL_Renderer* renderer) const override {
      Compass alignment{SOUTHWEST};
      Compass opposite{NORTHEAST};

      Region2D hport = grid.CalculateGridRegion(1, 1, 1, ports.y);
      Region2D vport = grid.CalculateGridRegion(size.x - ports.x+1, size.y, size.x, size.y);

      Position2D hportn = hport.NorthWest();
      Position2D hports = hport.SouthWest();
      Position2D vportw = vport.SouthWest();
      Position2D vporte = vport.SouthEast();

      Region2D ocorner = grid.bounds.Align(alignment, Size2D{outer_radius});

      Region2D icorner = grid.bounds.Align(opposite, Size2D{
          vportw.x - hports.x,
          vportw.y - hports.y
        });

      Region2D iradius = icorner.Align(alignment, Size2D{inner_radius});

      grid.bounds.Fill(renderer, Colours().frame);

      ocorner.Fill(renderer, Colours().background);
      ocorner.QuadrantArc(renderer, alignment, Colours().frame);

      icorner.Fill(renderer, Colours().background);
      iradius.Fill(renderer, Colours().frame);
      iradius.QuadrantArc(renderer, alignment, Colours().background);
    }
};

class NorthWestSweep : public Sweep {
  public:
    NorthWestSweep(Window& window, Grid grid, Vector2D size, Vector2D ports, int oradius, int iradius)
      : Sweep{window, grid, size, ports, oradius, iradius}
    {}

    void Draw(SDL_Renderer* renderer) const override {
      Compass alignment{NORTHWEST};
      Compass opposite{SOUTHEAST};

      Region2D hport = grid.CalculateGridRegion(1, 1, 1, ports.y);
      Region2D vport = grid.CalculateGridRegion(size.x - ports.x+1, size.y, size.x, size.y);

      Position2D hportn = hport.NorthWest();
      Position2D hports = hport.SouthWest();
      Position2D vportw = vport.SouthWest();
      Position2D vporte = vport.SouthEast();

      Region2D ocorner = grid.bounds.Align(alignment, Size2D{outer_radius});

      Region2D icorner = grid.bounds.Align(opposite, Size2D{
          vportw.x - hports.x,
          vportw.y - hports.y
        });

      Region2D iradius = icorner.Align(alignment, Size2D{inner_radius});

      grid.bounds.Fill(renderer, Colours().frame);

      ocorner.Fill(renderer, Colours().background);
      ocorner.QuadrantArc(renderer, alignment, Colours().frame);

      icorner.Fill(renderer, Colours().background);
      iradius.Fill(renderer, Colours().frame);
      iradius.QuadrantArc(renderer, alignment, Colours().background);
    }
};
