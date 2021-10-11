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

    void Add(Coordinate* c) {
      this->x += c->x;
      this->y += c->y;
    }

    void Subtract(Coordinate* c) {
      this->x -= c->x;
      this->y -= c->y;
    }
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

    Region(Position n, Position f)
        : position{n}, size{ f.x - n.x,
                             f.y - n.y }
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


    Position Centre() const {
      return Position{
          position.x + (size.x / 2),
          position.y + (size.y / 2)
        };
    }


    Position North() const {
      return Position{ Centre().x, Near().y };
    }

    Position East() const {
      return Position{ Far().x, Centre().y };
    }

    Position South() const {
      return Position{ Centre().x, Far().y };
    }

    Position West() const {
      return Position{ Near().x, Centre().y };
    }


    Position NorthEast() const {
      return Position{ Far().x, Near().y };
    }

    Position SouthEast() const {
      return Far();
    }

    Position SouthWest() const {
      return Position{ Near().x, Far().y };
    }

    Position NorthWest() const {
      return Near();
    }


    Region NorthEastQuadrent() const {
      return Region{ North(), East() };
    }

    Region SouthEastQuadrent() const {
      return Region{ Centre(), SouthEast() };
    }

    Region SouthWestQuadrent() const {
      return Region{ West(), South() };
    }

    Region NorthWestQuadrent() const {
      return Region{ NorthWest(), Centre() };
    }


    bool Encloses(Coordinate& point) const {
      Position near = this->Near(),
               far  = this->Far();

      return ( near.x <= point.x )
          && ( near.y <= point.y )
          && ( point.x <= far.x  )
          && ( point.y <= far.y  );
    };
};
