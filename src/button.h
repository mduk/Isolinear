#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "types.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"


class Button : public Drawable {
  public:
    Button(Region, ColourScheme, Label);
    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    Region Bounds();

  protected:
    TTF_Font* sdl_font;
    Region bounds;
    ColourScheme colours;
    Label label;
    bool active = false;
};

