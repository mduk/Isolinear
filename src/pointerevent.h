#pragma once

#include "geometry.h"

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
      : position{ws.x * e.x, ws.y * e.y},
        type{FINGER} {};

    Position2D Position() {
      return position;
    }
};
