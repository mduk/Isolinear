#include "elbo.h"


void Elbo::OnMouseButtonDown(SDL_MouseButtonEvent& e) {
}

void Elbo::Draw(SDL_Renderer* renderer) const {
  int max_cols = grid.MaxColumns();
  int max_rows = grid.MaxRows();

  GridRegion horizontal = grid.MultiCellRegion(4,1, max_cols,2);
  GridRegion sweep = grid.MultiCellRegion(1,1, 3,2);
  GridRegion vertical = grid.MultiCellRegion(1,3, 2,max_rows);

  Region2D reach{
      horizontal.Origin(),
      Size2D{
        horizontal.W(),
        reach_weight
      }
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

  Quad sweepq{ sweep, colours.base };
  Quad reachq{ reach, colours.base };
  Quad headerq{ header, colours.base };
  Quad verticalq{ vertical, colours.base };

  sweepq.Draw(renderer);
  reachq.Draw(renderer);
  headerq.Draw(renderer);
  verticalq.Draw(renderer);
}

