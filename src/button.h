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


class Button : public Region2D {
  public:
    Button(
        Window& w,
        Region2D b,
        ColourScheme cs,
        Label l
      ) :
        Region2D{b},
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
          Region2D{_position, _size},
          label
        );
    }


    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      printf("OnMouseButtonDown\n");
      Region2D::OnMouseButtonDown(event);
      active = !active;
    }

  protected:
    Window window;
    ColourScheme colours;
    Label label;
    bool active = false;
};

