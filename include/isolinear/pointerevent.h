#pragma once

#include "geometry.h"



namespace isolinear::pointer {
  namespace geometry = isolinear::geometry;
  namespace pointer = isolinear::pointer;

  using isolinear::geometry::Position2D;


  enum type {
    MOUSE, FINGER
  };


  class event {
    protected:
      Position2D position;
      pointer::type type;

    public:
      event(SDL_MouseButtonEvent e) : position{e.x, e.y}, type{MOUSE}  {};
      event(SDL_TouchFingerEvent e, geometry::vector ws)
        : position{
              static_cast<int>(ws.x * e.x),
              static_cast<int>(ws.y * e.y)
            },
          type{FINGER} {};

      Position2D Position() {
        return position;
      }
  };

}
