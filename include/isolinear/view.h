#pragma once

#include <string>

#include "grid.h"
#include "drawable.h"
#include "geometry.h"


class View : public Drawable {
  protected:
    std::string title;
    Grid grid;

  public:
    View(std::string t, Grid g) : title{t}, grid{g} {}

    std::string Name() const {
      return title;
    }

    Region2D Bounds() const {
      return grid.bounds;
    }

};
