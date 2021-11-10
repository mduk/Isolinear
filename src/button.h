#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "miso.h"

#include "window.h"
#include "drawable.h"
#include "grid.h"
#include "geometry.h"
#include "pointerevent.h"


using namespace std;


class Button : public Drawable {
  protected:
    Window& window;
    bool enabled = true;
    bool active = false;

  public:
    miso::signal<> signal_press;

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

    void Enable() { enabled = true; }
    void Disable() { enabled = false; }
    bool Enabled() { return enabled; }

    void Activate() { active = true; }
    void Deactivate() { active = false; }
    bool Active() { return active; }

    std::string Label() { return label; }
    void Label(std::string newlabel) { label = newlabel + " "; }

    virtual Region2D Bounds() const override {
      return bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      Colour drawcolour = enabled == true
                        ? active == true
                          ? Colours().active
                          : Colours().light_alternate
                        : Colours().disabled;


      boxColor(renderer,
          bounds.NearX(), bounds.NearY(),
          bounds.FarX(),  bounds.FarY(),
          drawcolour
        );

      window.ButtonFont().RenderText(
          renderer,
          Region2D{bounds.Origin(), bounds.Size()},
          Compass{SOUTHEAST},
          label
        );
    }


    void OnPointerEvent(PointerEvent event) {
      emit signal_press();
    }
};

