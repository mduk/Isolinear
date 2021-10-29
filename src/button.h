#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "drawable.h"
#include "grid.h"
#include "geometry.h"


using namespace std;


class Button : public Region2D {
  protected:
    Window window;
    bool active = false;
    bool left_cap = true;
    bool right_cap = true;

  public:
    std::string label;
    Button(
        Window& w,
        Region2D b,
        ColourScheme cs,
        std::string l
      ) :
        Region2D{b},
        window{w},
        label{l}
    {}

    Region2D LeftCapRegion() const {
      return Region2D{
          _position,
          Size2D{_size.y}
        };
    }

    Region2D RightCapRegion() const {
      return Region2D{
          Position2D{
              _position.x + (_size.x - _size.y),
              _position.y
            },
          Size2D{_size.y}
        };
    }

    void Draw(SDL_Renderer* renderer) const {
      Colour drawcolour = active == true
                        ? Colours().active
                        : Colours().light_alternate;


      boxColor(renderer,
          NearX(), NearY(),
          FarX(),  FarY(),
          drawcolour
        );

      if (left_cap) {
        Region2D left_cap = LeftCapRegion();
        //left_cap.Draw(renderer);
      }

      if (right_cap) {
        Region2D right_cap = RightCapRegion();
        //right_cap.Draw(renderer);
      }

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

