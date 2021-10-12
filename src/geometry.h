#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>



class Coordinate {
  public:
    int x, y;

    Coordinate()
        : x{0}, y{0}
      {};

    Coordinate(int _x, int _y)
        : x{_x}, y{_y}
      {};

    Coordinate(SDL_MouseButtonEvent e)
        : x{e.x}, y{e.y}
      {};

    Coordinate(SDL_MouseMotionEvent e)
        : x{e.x}, y{e.y}
      {};

    Coordinate(SDL_Surface* s)
        : x{s->w}, y{s->h}
      {};

    void Add(Coordinate c) {
      this->x += c.x;
      this->y += c.y;
    }

    void Subtract(Coordinate c) {
      this->x -= c.x;
      this->y -= c.y;
    }
};


class Position : public Coordinate {
  public:
    Position() : Coordinate() {}
    Position(int x, int y) : Coordinate(x, y) {};
    Position(SDL_MouseButtonEvent e) : Coordinate(e) {};
};

class Size : public Coordinate {
  public:
    Size() : Coordinate() {};
    Size(int w, int h) : Coordinate(w, h) {};
    Size(SDL_Surface* s) : Coordinate(s) {};
};


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

    // Near and Far Point Positions

    Position Near()  const { return this->position; }
         int NearX() const { return this->position.x; }
         int NearY() const { return this->position.y; }

    Position Far()  const { return Position{ FarX(), FarY() }; }
         int FarX() const { return this->position.x + this->size.x; }
         int FarY() const { return this->position.y + this->size.y; }

    // Centre Point Position

    Position Centre()  const { return Position{ position.x + (size.x / 2),
                                                position.y + (size.y / 2) }; }
         int CentreX() const { return Centre().x; }
         int CentreY() const { return Centre().y; }

    // Compass Point Positions

    Position North()  const { return Position{ Centre().x, Near().y }; }
         int NorthX() const { return North().x; }
         int NorthY() const { return North().y; }

    Position East()   const { return Position{ Far().x, Centre().y }; }
         int EastX()  const { return East().x; }
         int EastY()  const { return East().y; }

    Position South()  const { return Position{ Centre().x, Far().y }; }
         int SouthX() const { return South().x; }
         int SouthY() const { return South().y; }

    Position West()   const { return Position{ Near().x, Centre().y }; }
         int WestX()  const { return West().x; }
         int WestY()  const { return West().y; }

    Position NorthEast()  const { return Position{ Far().x, Near().y }; }
         int NorthEastX() const { return NorthEast().x; }
         int NorthEastY() const { return NorthEast().y; }

    Position SouthEast()  const { return Far(); }
         int SouthEastX() const { return SouthEast().x; }
         int SouthEastY() const { return SouthEast().y; }

    Position SouthWest()  const { return Position{ Near().x, Far().y }; }
         int SouthWestX() const { return SouthWest().x; }
         int SouthWestY() const { return SouthWest().y; }

    Position NorthWest()  const { return Near(); }
         int NorthWestX() const { return NorthWest().x; }
         int NorthWestY() const { return NorthWest().y; }

    // Compass Point Regions

    Region SouthEast(Size& s) const {
      return Region{
          SouthEastX() - s.x,
          SouthEastY() - s.y,
          s.x,
          s.y
        };
    }

    // Compass Quadrants
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
      return ( NearX() <= point.x )
          && ( NearY() <= point.y )
          && ( point.x <= FarX()  )
          && ( point.y <= FarY()  );
    };



    SDL_Rect AsSdlRect() const {
      return SDL_Rect{
          position.x,
          position.y,
          size.x,
          size.y
        };
    }
};
