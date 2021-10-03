#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawing.h"
#include "geometry.h"
#include "colours.h"

class Cap {
  public:
    Cap(Colour _colour) : bg_colour{_colour} {};
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );

  protected:
    Colour bg_colour;
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
    LeftRoundCap(Colour _colour) : Cap{_colour} {};
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );
};

class RightRoundCap : public Cap {
  public:
    RightRoundCap(Colour _colour) : Cap{_colour} {};
    void Draw(
      SDL_Renderer* renderer,
      Region* where
    );
};

class Button {
  public:
    bool lcap = false;
    bool rcap = false;
    Colour c;

    Button(Region* _b)
      : region{_b},
        c{0xFF0099FF}
    {};

    void Draw(
        SDL_Renderer* renderer
      );
    void OnMouseButtonDown(
        SDL_MouseButtonEvent* event
      );

  private:
    Region* region;
    std::string label = "defaultlabel";
    int g;
};
