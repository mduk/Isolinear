#include "grid.h"

Grid::Grid(
    int _ncols,
    int _nrows,
    int _rowh,
    int _roww,
    int _gutter
) : ncols{_ncols},
    nrows{_nrows},
    rowh{_rowh},
    roww{_roww},
    gutter{_gutter} { }

void Grid::Draw(
    SDL_Renderer* renderer,
    Position* gridpos
) {
  int gutter = 10;

  for (int row_n = 1; row_n <= this->nrows; row_n++) {
    GridRow* row = new GridRow(this);
    Region* row_region = new Region(
      gridpos->x,
      gridpos->y + ((this->rowh + gutter) * (row_n - 1)),
      this->roww, this->rowh
    );
    row->Draw(renderer, row_region);
    delete row, row_region;
  }
}

void GridRow::Draw(SDL_Renderer* renderer, Region* where) {
  rectangleColor(renderer,
    where->position->x,
    where->position->y,
    where->position->x + where->size->x,
    where->position->y + where->size->y,
    0xFFFFFFFF
  );

  int gutter = 10;
  int btnw = where->size->y * 1.618;
  for (int col = 1; col <= this->grid->ncols; col++) {
    Region* btnregion = new Region(
      (where->position->x + ((col - 1) * (btnw + gutter))),
      where->position->y,
      btnw,
      where->size->y
    );

    Button* btn = new Button();
    if (col == 1                ) btn->lcap = true;
    if (col == this->grid->ncols) btn->rcap = true;
    btn->Draw(renderer, btnregion);

    delete btn, btnregion;
  }
}
