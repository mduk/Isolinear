#include "grid.h"

Grid::Grid(
    int _ncols,
    int _nrows,
    int _rowh
) : ncols{_ncols},
    nrows{_nrows},
    rowh{_rowh} { }

void Grid::Draw(
    SDL_Renderer* renderer,
    Position* gridpos
) {
  int btnw = this->rowh * 1.618;
  int gutter = 10;

  for (int row = 1; row <= this->nrows; row++) {
    for (int col = 1; col <= this->ncols; col++) {
      Region* btnregion = new Region(
        (gridpos->x + ((col - 1) * (btnw       + gutter))),
        (gridpos->y + ((row - 1) * (this->rowh + gutter))),
        btnw,
        this->rowh
      );

      Button* btn = new Button();
      if (col == 1) {
        printf("col one\n");
        btn->lcap = true;
      }
      if (col == this->ncols) btn->rcap = true;
      btn->Draw(renderer, btnregion);

      delete btn, btnregion;
    }
  }
}
