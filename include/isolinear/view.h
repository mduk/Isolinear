#pragma once

#include <string>

#include "layout.h"
#include "control.h"
#include "geometry.h"




namespace isolinear {

  class view : public ui::control {
    protected:
      std::string title;
      layout::grid grid;

    public:
      view(std::string t, layout::grid g) : title{t}, grid{g} {}

      std::string Name() const {
        return title;
      }

      geometry::region bounds() const {
        return grid.bounds();
      }

  };


}
