#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "drawable.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"


using namespace std;


class Button : public Region2D {
  protected:
    Window window;
    ColourScheme colours;
    std::string label;
    bool active = false;

  public:
    Button(
        Window& w,
        Region2D b,
        ColourScheme cs,
        std::string l
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

      window.ButtonFont().RenderText(
          renderer,
          Region2D{_position, _size},
          Compass{SOUTHEAST},
          label
        );
    }


    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      printf("OnMouseButtonDown\n");
      Region2D::OnMouseButtonDown(event);
      active = !active;
    }
};

