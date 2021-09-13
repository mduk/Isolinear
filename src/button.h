#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Cap {
  public:
    Cap(Uint32 _colour) : bg_colour{_colour} {};
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );

  protected:
    Uint32 bg_colour;
};

class RightBarCap : public Cap {
  public:
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );
};

class LeftIndicatorCap : public Cap {
  public:
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );
};

class LeftRoundCap : public Cap {
  public:
    LeftRoundCap(Uint32 _colour) : Cap{_colour} {};
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );
};

class RightRoundCap : public Cap {
  public:
    RightRoundCap(Uint32 _colour) : Cap{_colour} {};
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );
};

class Button {
  public:
    Button();
    void Draw(SDL_Renderer* renderer, Region* where);
    bool lcap = false;
    bool rcap = false;

  private:
    std::string label = "defaultlabel";
    int g;
    Uint32 c;
};
