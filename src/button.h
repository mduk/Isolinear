#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "drawable.h"
#include "types.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"


class Button : public Region {
  public:
    Button(
        Window& w,
        Region b,
        ColourScheme cs,
        Label l
      ) :
        Region{b},
        window{w},
        colours{cs},
        label{l}
    {}

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);

  protected:
    Window window;
    ColourScheme colours;
    Label label;
    bool active = false;
};

