#pragma once

#include <string>

#include "grid.h"
#include "drawable.h"
#include "geometry.h"




namespace isolinear {

  class View : public ui::drawable {
    protected:
      std::string title;
      isolinear::grid grid;

    public:
      View(std::string t, isolinear::grid g) : title{t}, grid{g} {}

      std::string Name() const {
        return title;
      }

      geometry::Region2D Bounds() const {
        return grid.bounds;
      }

  };


}
