#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <miso.h>

#include "colours.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"
#include "window.h"


namespace isolinear::ui {

  // Widgets

  class horizontal_rule : public drawable {

    protected:
      Window& window;
      Grid grid;

    public:
      horizontal_rule(Window& w, Grid g)
        : window(w)
        , grid(g)
      {}

    public:
      Region2D Bounds() const {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const {
        auto bound_height = grid.bounds.H();
        auto offset_px = (bound_height - grid.gutter.y) / 2;

        auto hrule_near_x = grid.bounds.NearX();
        auto hrule_near_y = grid.bounds.NearY() + offset_px;

        auto hrule_far_x = grid.bounds.FarX();
        auto hrule_far_y = grid.bounds.FarY() - offset_px;

        Region2D hrule(
            Position2D(hrule_near_x, hrule_near_y),
            Position2D(hrule_far_x,  hrule_far_y)
          );

        hrule.Fill(renderer, Colours().frame);
      }
  };


  class button : public drawable {
    protected:
      Window& window;
      bool enabled = true;
      bool active = false;
      std::string label;

    public:
      miso::signal<> signal_press;
      Region2D bounds;

      button( Window& w, Grid g, std::string l)
        : button(w, g.bounds, l)
      {}

      button(
          Window& w,
          Region2D b,
          std::string l
        ) :
          bounds{b},
          window{w},
          label{l}
      {}

      void Enable() { enabled = true; }
      void Disable() { enabled = false; }
      bool Enabled() { return enabled; }
      bool Disabled() { return !enabled; }

      void Activate() { active = true; }
      void Deactivate() { active = false; }
      void Active(bool state) { active = state; }
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
            bounds,
            Compass::SOUTHEAST,
            std::string{" "} + label + " "
          );
      }


      void OnPointerEvent(PointerEvent event) {
        emit signal_press();
      }
  };


}
