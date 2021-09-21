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

  std::list<GridCell*> cells;
  cells.push_back(new GridCell(0xff0ff0ff, where->size->y));
  cells.push_back(new GridCell(0xff438ba4, where->size->y * 1.618));
  cells.push_back(new GridCell(0xff000000, where->size->y * 1.618 * 3));
  cells.push_back(new GridCell(0xfff00f00, where->size->y * 1.618));
  cells.push_back(new GridCell(0xff0fffa0, where->size->y * 1.618 * 2));
  cells.push_back(new GridCell(0xff438ba4, where->size->y * 1.618));
  cells.push_back(new GridCell(0xff0ff0ff, where->size->y));

  int x = where->position->x;
  int y = where->position->y;

  for (auto const& c : cells) {
    Region* cell_region = new Region(
      x, y,
      c->width, where->size->y
    );
    c->Draw(renderer, cell_region);
    delete cell_region;

    x += c->width;
  }
}

void GridCell::Draw(SDL_Renderer* renderer, Region* where) {
  rectangleColor(renderer,
    where->position->x,
    where->position->y,

    where->position->x + where->size->x,
    where->position->y + where->size->y,

    this->colour
  );

  int g = 6;
  boxColor(renderer,
    where->position->x + g,
    where->position->y + g,

    where->position->x + where->size->x - g,
    where->position->y + where->size->y - g,

    this->colour
  );
}
