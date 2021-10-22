#include "elbo.h"


void Elbo::OnMouseButtonDown(SDL_MouseButtonEvent& e) {
}

void Elbo::Draw(SDL_Renderer* renderer) const {
  int sweep_w = 3;
  int sweep_h = 2;

  int max_cols = grid.MaxColumns();
  int max_rows = grid.MaxRows();

  GridRegion sweep = grid.MultiCellRegion(
      1,1,
      sweep_w,sweep_h
    );
  GridRegion horizontal = grid.MultiCellRegion(
      sweep_w + 1,1,
      max_cols,2
    );
  GridRegion vertical = grid.MultiCellRegion(
      1,sweep_h + 1,
      sweep_w-1,max_rows
    );
  GridRegion container = grid.MultiCellRegion(
      sweep_w, sweep_h + 1,
      max_cols, max_rows
    );

  Region2D outer_radius{
      sweep.Origin(),
      Size2D{sweep.H()}
    };

  Region2D header{
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

  Region2D inner_corner = sweep.AlignSouthEast(
      Size2D{
          sweep.FarX() - vertical.FarX(),
          header.H() + gutter.y
        }
    );

  Region2D inner_radius = inner_corner.AlignNorthWest(
      Size2D{50}
    );

  Region2D reach{
      horizontal.Origin(),
      Size2D{
        horizontal.W(),
        reach_weight
      }
    };

  Quad sweepq{ sweep, colours.base };
  Quad reachq{ reach, colours.base };
  Quad headerq{ header, 0xff000000 };
  Quad inner_cornerq{ inner_corner, 0xff000000 };
  Quad inner_radiusq{ inner_radius, 0xaa00ffff };
  Quad verticalq{ vertical, colours.base };
  Quad containerq{ container, colours.disabled };

  sweepq.Draw(renderer);
  outer_radius.Draw(renderer);
  inner_cornerq.Draw(renderer);
  inner_radiusq.Draw(renderer);
  reachq.Draw(renderer);
  headerq.Draw(renderer);
  verticalq.Draw(renderer);
  containerq.Draw(renderer);
}
