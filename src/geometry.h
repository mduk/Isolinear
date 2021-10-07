#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"

class Region;



class Coordinate {
  public:
    int x, y;

    Coordinate(int _x, int _y)
        : x{_x}, y{_y}
      {};
    Coordinate(Coordinate& _c)
        : x{_c.x}, y{_c.y}
      {};

    void Add(Coordinate* c);
    void Subtract(Coordinate* c);

    bool IsWithin(
        Region* region
      );
};



class Position : public Coordinate {
  public:
    Position(int _x, int _y)
        : Coordinate{_x, _y}
      { };

    void Draw(
        SDL_Renderer* r,
        Colour c
      );
    void Draw(
        SDL_Renderer* r
      );
};



class Size : public Coordinate {
  public:
    Size(int _x, int _y)
        : Coordinate{_x, _y}
      { };
};



class Region {
  public:
    Position position;
    Size size;

    Region( Position _p, Size _s)
        : position{_p}, size{_s}
      {};

    Region( int _x, int _y, int _w, int _h)
        : position{_x, _y}, size{_w, _h}
      {};

    bool Encloses(Coordinate& point);

    Region* Copy();

    Region* Draw(
        SDL_Renderer* renderer
      );
    Region* Draw(
        SDL_Renderer* renderer,
        Colour colour
      );
};



class WindowRegion : public Region {
  public:
    WindowRegion(SDL_Window* _window);

    void Update();

  protected:
    SDL_Window* window;
};
