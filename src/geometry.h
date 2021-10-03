#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"

class Region;

class Coordinate {
  public:
    unsigned int x, y;
    Coordinate* origin;

    Coordinate(
        unsigned int x,
        unsigned int y
      );
    Coordinate* Copy();
    Coordinate* Extend(
        unsigned int x,
        unsigned int y
      );

    unsigned int X();
    unsigned int Y();

    void Add(Coordinate* c);
    void Subtract(Coordinate* c);

    bool IsWithin(
        Region* region
      );
};

class Position : public Coordinate {
  public:
    Position(
        unsigned int x,
        unsigned int y
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
        unsigned int w,
        unsigned int h
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
        unsigned int x,
        unsigned int y,
        unsigned int w,
        unsigned int h
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
