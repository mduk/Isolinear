#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "types.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"


class Button : public Drawable {
  public:
    Button(Region _b, ColourScheme _cs)
      : bounds{_b}, colours{_cs}
    {};

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    Region Bounds();

  protected:
    Region bounds;
    ColourScheme colours;
    bool active = false;
};

