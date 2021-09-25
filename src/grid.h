#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"
#include "colours.h"

class GridCell {
  public:
    int width;
    Colour colour;
    Button* button;

    GridCell(Button* _b, int _w) : button{_b}, width{_w} {};
    void Draw(
        SDL_Renderer* renderer,
        Region* where
      );
};

class GridRow;

class Grid {
  public:
    Region* where;
    int rowh;
    int gutter;

    Grid(
        Region* where,
        int rowh,
        int gutter
      );
    void Draw(SDL_Renderer* renderer);
    Region* CalculateCellRegion(int row, int left_col, int right_col);
    void OnMouseButtonDown(SDL_MouseButtonEvent* event);

  protected:
    int num_cols = 12;
    std::list<GridRow*> rows;
};

class GridRow {
  public:
    GridRow(Grid* _g) : grid{_g} {};
    void Draw(
        SDL_Renderer* renderer,
        Region* where
      );

  protected:
    Grid* grid;
    std::list<GridCell*> cells;
};

class TitleRow : public GridRow {
  public:
    TitleRow(Grid* _g) : GridRow{_g} {
      Button* cap = new Button();
      cap->lcap = true;
      cap->c = RandomColour();
      this->cells.push_back(new GridCell(cap, this->grid->rowh));

      Button* button = new Button();
      button->c = RandomColour();
      this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 12));

      Button* rrcap = new Button();
      rrcap->rcap = true;
      rrcap->c = RandomColour();
      this->cells.push_back(new GridCell(rrcap, this->grid->rowh));
    };
};

class ContainerRow : public GridRow {
  public:
    ContainerRow(Grid* _g);
};

class ImageRow : public GridRow {
  public:
    ImageRow(Grid* _g);
};

class VolumeRow : public GridRow {
  public:
    VolumeRow(Grid* _g);
};

class Volume2Row : public GridRow {
  public:
    Volume2Row(Grid* _g);
};

class Volume3Row : public GridRow {
  public:
    Volume3Row(Grid* _g);
};
