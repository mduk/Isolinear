#pragma once

#include <string>

#include "layout.h"
#include "control.h"
#include "geometry.h"




namespace isolinear {

  class view : public ui::control {
    protected:
      std::string m_name;

    public:
      view(std::string t, layout::grid g)
        : ui::control(g)
        , m_name(t) {}

      std::string name() const {
        return m_name;
      }

      virtual void on_activate() {}

  };


}
