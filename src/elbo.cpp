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
      Size2D{150}
    );

  Region2D reach{
      horizontal.Origin(),
      Size2D{
        horizontal.W(),
        reach_weight
      }
    };

  boxColor(renderer,
      sweep.NearX(), sweep.NearY(),
      sweep.FarX(), sweep.FarY(),
      colours.base
    );

  boxColor(renderer,
      inner_corner.NearX(), inner_corner.NearY(),
      inner_corner.FarX(), inner_corner.FarY(),
      0xff000000
    );

  boxColor(renderer,
      inner_radius.NearX(), inner_radius.NearY(),
      inner_radius.FarX(), inner_radius.FarY(),
      colours.base
    );

  filledPieColor(renderer,
      inner_radius.FarX(), inner_radius.FarY(),
      inner_radius.H(),
      180, 270,
      0xff000000
    );

  boxColor(renderer,
      outer_radius.NearX(), outer_radius.NearY(),
      outer_radius.FarX(), outer_radius.FarY(),
      0xff000000
    );

  filledPieColor(renderer,
      outer_radius.FarX(), outer_radius.FarY(),
      outer_radius.H(),
      180, 270,
      colours.base
    );

  boxColor(renderer,
      reach.NearX(), reach.NearY(),
      reach.FarX(), reach.FarY(),
      colours.base
    );

  boxColor(renderer,
      vertical.NearX(), vertical.NearY(),
      vertical.FarX(), vertical.FarY(),
      colours.base
    );
}
