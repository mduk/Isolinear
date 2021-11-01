#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Sweep : public Region2D {
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
    {
      printf("Sweep Grid: %d,%d\n", grid.MaxColumns(), grid.MaxRows());
    }

    void Draw(SDL_Renderer* renderer) const override {
      Position2D near_corner = grid.bounds.Near();
      Position2D  far_corner = grid.bounds.Far();

        Region2D hport  = grid.CalculateGridRegion(size.x, 1, size.x, ports.y);
      Position2D hportn = hport.NorthEast();
      Position2D hports = hport.SouthEast();

        Region2D vport  = grid.CalculateGridRegion(1, size.y, ports.x, size.y);
      Position2D vportw = vport.SouthWest();
      Position2D vporte = vport.SouthEast();

      Region2D ocorner = grid.bounds.Align(Compass::NORTHWEST, Size2D{outer_radius});
      Region2D icorner = grid.bounds.Align(Compass::SOUTHEAST, Size2D{
          far_corner.x - vporte.x,
          far_corner.y - hports.y
        });

      Region2D iradius = icorner.Align(Compass::NORTHWEST, Size2D{inner_radius});

      grid.bounds.Fill(renderer, Colours().frame);

      ocorner.Fill(renderer, Colours().background);
      ocorner.QuadrantArc(renderer, Compass::NORTHWEST, Colours().frame);

      icorner.Fill(renderer, Colours().background);
      iradius.Fill(renderer, Colours().frame);
      iradius.QuadrantArc(renderer, Compass::NORTHWEST, Colours().background);

      if (false) {
        hportn.Draw(renderer);
        hports.Draw(renderer);
        vportw.Draw(renderer);
        vporte.Draw(renderer);
      }
    }
};
