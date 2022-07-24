#pragma once

#include <string>

#include "grid.h"
#include "control.h"
#include "geometry.h"




namespace isolinear {

  class view : public ui::control {
    protected:
      std::string title;
      isolinear::grid grid;

    public:
      view(std::string t, isolinear::grid g) : title{t}, grid{g} {}

      std::string Name() const {
        return title;
      }

      geometry::region bounds() const {
        return grid.bounds();
      }

  };


}
