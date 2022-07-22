#pragma once

#include "geometry.h"


namespace isolinear::pointer {

  using isolinear::geometry::position;


  enum type {
    MOUSE, FINGER
  };


  class event {
    protected:
      position m_position;
      pointer::type m_type;

    public:
      explicit event(SDL_MouseMotionEvent e) : m_position{e.x, e.y}, m_type{MOUSE} {};
      explicit event(SDL_MouseButtonEvent e) : m_position{e.x, e.y}, m_type{MOUSE}  {};

      [[maybe_unused]] event(SDL_TouchFingerEvent e, geometry::vector ws)
        : m_position{
              static_cast<int>(ws.x * e.x),
              static_cast<int>(ws.y * e.y)
            },
          m_type{FINGER} {};

      position Position() {
        return m_position;
      }
  };

}
