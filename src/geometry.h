#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"

class Region;



class Coordinate {
  public:
    int x, y;

    Coordinate( int _x, int _y)
        : x{_x}, y{_y}
      {};
    Coordinate( Coordinate* _c)
        : x{_c->x}, y{_c->y}
      {};

    void Add(Coordinate* c);
    void Subtract(Coordinate* c);

    bool IsWithin(
        Region* region
      );
};



class Position : public Coordinate {
  public:
    Position(
        int x,
        int y
      );

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
    Size(
        int w,
        int h
      );
};



class Region {
  public:
    Region* origin;
    Position* position;
    Size* size;

    Region(
        Position* p,
        Size* s
      );
    Region(
        Position* p,
        Position* s
      );
    Region(
        int x,
        int y,
        int w,
        int h
      );

    bool Encloses(
        Coordinate* point
      );

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
