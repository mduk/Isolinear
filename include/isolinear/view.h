#pragma once

#include <string>

#include "layout.h"
#include "control.h"
#include "geometry.h"




namespace isolinear {

  class view : public ui::control {
    protected:
      std::string m_name;
      layout::grid m_grid;

    public:
      view(std::string t, layout::grid g) : m_name{t}, m_grid{g} {}

      std::string Name() const {
        return m_name;
      }

      geometry::region bounds() const {
        return m_grid.bounds();
      }

  };


}
