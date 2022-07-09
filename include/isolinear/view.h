#pragma once

#include <string>

#include "grid.h"
#include "drawable.h"
#include "geometry.h"




namespace isolinear {

  class view : public ui::drawable {
    protected:
      std::string title;
      isolinear::grid grid;

    public:
      view(std::string t, isolinear::grid g) : title{t}, grid{g} {}

      std::string Name() const {
        return title;
      }

      geometry::region Bounds() const {
        return grid.bounds;
      }

  };


}
