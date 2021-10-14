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


class Button : public Drawable {
  public:
    Button(
        Window& w,
        Region b,
        ColourScheme cs,
        Label l
      ) :
        window{w},
        bounds{b},
        colours{cs},
        label{l}
    {}

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    Region Bounds() {
      return bounds;
    }

  protected:
    Window window;
    Region bounds;
    ColourScheme colours;
    Label label;
    bool active = false;
};

