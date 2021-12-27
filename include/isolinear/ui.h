#pragma once

#include <list>
#include <map>
#include <string>

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


using isolinear::geometry::Region2D;


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


  class button_bar : public drawable {
    protected:
      Grid grid;
      Window& window;
      std::map<std::string, isolinear::ui::button> buttons;
      Vector2D button_size{2,2};

    public:
      button_bar(Window& w, Grid g)
        : window{w}, grid{g}
      {};

      virtual Region2D ButtonRegion(int i) const = 0;
      virtual Region2D BarRegion() const = 0;

      virtual ColourScheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(ColourScheme cs) {
        for (auto& [label, button] : buttons) {
          button.Colours(cs);
        }
        drawable::Colours(cs);
      }

      virtual isolinear::ui::button& AddButton(std::string label) {
        buttons.try_emplace(
            label,
              window,
              ButtonRegion(buttons.size() + 1),
              label
          );
        return buttons.at(label);
      }

      virtual isolinear::ui::button& GetButton(std::string label) {
        return buttons.at(label);
      }

      virtual void DeactivateAll() {
        for (auto& [label, button] : buttons) {
          button.Deactivate();
        }
      }

      virtual void ActivateOne(std::string label) {
        buttons.at(label).Activate();
      }

      virtual void OnPointerEvent(PointerEvent event) override {
        for (auto& [label, button] : buttons) {
          if (button.Bounds().Encloses(event.Position())) {
            button.OnPointerEvent(event);
            return;
          }
        }
      };

      virtual int Height() const {
        return grid.MaxRows();
      }

      virtual int Width() const {
        return grid.MaxColumns();
      }

      virtual Region2D Bounds() const override {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const override {
        for (auto const& [label, button] : buttons) {
          button.Draw(renderer);
        }

        Region2D bar = BarRegion();

        boxColor(renderer,
            bar.NearX(), bar.NearY(),
            bar.FarX(),  bar.FarY(),
            Colours().frame
          );
      }
  };


  class horizontal_button_bar : public button_bar {
    public:
      horizontal_button_bar(Window& w, Grid g) : button_bar(w, g) {}

      Region2D ButtonRegion(int i) const override {
        int near_col = button_size.x * (i-1) + 1;
        int near_row = 1;
        int  far_col = button_size.x * i;
        int  far_row = grid.MaxRows();

        return grid.CalculateGridRegion(
            near_col, near_row,
             far_col, far_row
          );
      }

      Region2D BarRegion() const override {
        int near_col = button_size.x * buttons.size() + 1;
        int near_row = 1;
        int  far_col = grid.MaxColumns();
        int  far_row = grid.MaxRows();

        return grid.CalculateGridRegion(
            near_col, near_row,
             far_col, far_row
          );
      }
  };


  class vertical_button_bar : public button_bar {
    public:
      vertical_button_bar(Window& w, Grid g) : button_bar(w, g) {}

      Region2D ButtonRegion(int i) const override {
        int near_col = 1;
        int near_row = button_size.y * (i-1) + 1;
        int  far_col = grid.MaxColumns();
        int  far_row = button_size.y * i;

        return grid.CalculateGridRegion(
            near_col, near_row,
             far_col, far_row
          );
      }

      Region2D BarRegion() const override {
        int near_col = 1;
        int near_row = button_size.y * buttons.size() + 1;
        int  far_col = grid.MaxColumns();
        int  far_row = grid.MaxRows();

        return grid.CalculateGridRegion(
            near_col, near_row,
             far_col, far_row
          );
      }
  };


}
