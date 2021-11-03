#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "drawable.h"
#include "grid.h"
#include "geometry.h"


using namespace std;


class Button : public Drawable {
  protected:
    Window& window;
    bool active = false;
    bool left_cap = true;
    bool right_cap = true;

  public:
    Region2D bounds;
    std::string label;
    Button(
        Window& w,
        Region2D b,
        ColourScheme cs,
        std::string l
      ) :
        bounds{b},
        window{w},
        label{l}
    {}

    Region2D LeftCapRegion() const {
      return Region2D{
          bounds.Origin(),
          Size2D{bounds.Size().y}
        };
    }

    Region2D RightCapRegion() const {
      return Region2D{
          Position2D{
              bounds.Origin().x + (bounds.Size().x - bounds.Size().y),
              bounds.Origin().y
            },
          Size2D{bounds.Size().y}
        };
    }

    void Draw(SDL_Renderer* renderer) const override {
      Colour drawcolour = active == true
                        ? Colours().active
                        : Colours().light_alternate;


      boxColor(renderer,
          bounds.NearX(), bounds.NearY(),
          bounds.FarX(),  bounds.FarY(),
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
          Region2D{bounds.Origin(), bounds.Size()},
          Compass{SOUTHEAST},
          label
        );
    }


    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      printf("Button::OnMouseButtonDown()\n");
      active = !active;
    }
};

