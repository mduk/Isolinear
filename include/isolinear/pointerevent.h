#pragma once

#include "geometry.h"

using isolinear::geometry::Position2D;
using isolinear::geometry::Size2D;

enum PointerType {
  MOUSE, FINGER
};

class PointerEvent {
  protected:
    Position2D position;
    PointerType type;

  public:
    PointerEvent(SDL_MouseButtonEvent e) : position{e.x, e.y}, type{MOUSE}  {};
    PointerEvent(SDL_TouchFingerEvent e, Size2D ws)
      : position{
            static_cast<int>(ws.x * e.x),
            static_cast<int>(ws.y * e.y)
          },
        type{FINGER} {};

    Position2D Position() {
      return position;
    }
};
