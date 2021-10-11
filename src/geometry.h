#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>



class Coordinate {
  public:
    int x, y;

    Coordinate(int _x, int _y)
        : x{_x}, y{_y}
      {};

    Coordinate(SDL_MouseButtonEvent e)
        : x{e.x}, y{e.y}
      {};

    Coordinate(SDL_MouseMotionEvent e)
        : x{e.x}, y{e.y}
      {};

    void Add(Coordinate* c);
    void Subtract(Coordinate* c);
};


using Position = Coordinate;
using Size = Coordinate;


class Region {
  public:
    Position position;
    Size size;

    Region()
        : position{0,0}, size{0,0}
      {};

    Region(Position _p, Size _s)
        : position{_p}, size{_s}
      {};

    Region(int _x, int _y, int _w, int _h)
        : position{_x, _y}, size{_w, _h}
      {};

    Position Near() const {
      return this->position;
    }

    Position Far() const {
      return Position{
        this->FarX(),
        this->FarY()
      };
    }

    int NearX() const {
      return this->position.x;
    }

    int NearY() const {
      return this->position.y;
    }

    int FarX() const {
      return this->position.x
           + this->size.x;
    }

    int FarY() const {
      return this->position.y
           + this->size.y;
    }

    bool Encloses(Coordinate& point);
};
