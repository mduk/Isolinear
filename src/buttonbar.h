#pragma once

#include <string>
#include <list>
#include <map>

#include "geometry.h"
#include "window.h"
#include "button.h"

class ButtonBar : public Drawable {
  protected:
    Grid grid;
    Window& window;
    std::map<std::string, Button> buttons;
    Vector2D button_size{2,2};

  public:
    ButtonBar(Window& w, Grid g)
      : window{w}, grid{g}
    {};

    virtual Region2D ButtonRegion(int i) const = 0;
    virtual Region2D BarRegion() const = 0;

    virtual ColourScheme Colours() const {
      return Drawable::Colours();
    }

    virtual void Colours(ColourScheme cs) {
      for (auto& [label, button] : buttons) {
        button.Colours(cs);
      }
      Drawable::Colours(cs);
    }

    virtual Button& AddButton(std::string label) {
      buttons.try_emplace(
          label,
            window,
            ButtonRegion(buttons.size() + 1),
            label
        );
      return buttons.at(label);
    }

    virtual void DeactivateAll() {
      for (auto& [label, button] : buttons) {
        button.Deactivate();
      }
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

class HorizontalButtonBar : public ButtonBar {
  public:
    HorizontalButtonBar(Window& w, Grid g) : ButtonBar(w, g) {}

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

class VerticalButtonBar : public ButtonBar {
  public:
    VerticalButtonBar(Window& w, Grid g) : ButtonBar(w, g) {}

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
