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

    void Draw(SDL_Renderer* renderer) const {
      Colour drawcolour = active == true
                        ? colours.active
                        : colours.base;
      boxColor(renderer,
          NearX(), NearY(),
          FarX(),  FarY(),
          drawcolour
        );

      window.ButtonFont().RenderTextSouthEast(
          renderer,
          Region{_position, _size},
          label
        );
    }


    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      printf("OnMouseButtonDown\n");
      Region::OnMouseButtonDown(event);
      active = !active;
    }

  protected:
    Window window;
    ColourScheme colours;
    Label label;
    bool active = false;
};

