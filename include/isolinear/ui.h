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


  class header_basic : public drawable {
    protected:
      Grid grid;
      Window& window;
      Compass alignment = Compass::CENTRE;
      std::string text{""};

    public:
      header_basic(Grid g, Window& w, Compass a)
        : header_basic(g, w, a, "")
      {}

      header_basic(Grid g, Window& w, Compass a, std::string t)
        : grid{g}, window{w}, alignment{a}, text{t}
      {}


      void Label(std::string newlabel) {
          text = newlabel;
      }
      virtual ColourScheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(ColourScheme cs) {
        drawable::Colours(cs);
      }

      virtual Region2D Bounds() const override {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const override {
        if (text.length() == 0) {
          grid.bounds.Fill(renderer, Colours().frame);
          return;
        }

        std::string padded = std::string(" ") + text + " ";

        RenderedText headertext = window.HeaderFont().RenderText(Colours().active, padded);
        headertext.Draw(renderer, alignment, grid.bounds);
      }
  };

  class header_east_bar : public drawable {
    protected:
      Grid grid;
      Window& window;
      std::string text{""};
      std::map<std::string, isolinear::ui::button> buttons;
      int button_width{2};

    public:
      header_east_bar(Grid g, Window& w, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      header_east_bar(Grid g, Window& w, Compass a, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      header_east_bar(Grid g, Window& w, Compass a)
        : grid{g}, window{w}
      {};

      void Label(std::string newlabel) {
          text = newlabel;
      }
      virtual ColourScheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(ColourScheme cs) {
        for (auto& [label, button] : buttons) {
          button.Colours(cs);
        }
        drawable::Colours(cs);
      }

      isolinear::ui::button& AddButton(std::string label) {
        buttons.try_emplace(
            label,
            window,
            ButtonRegion(buttons.size() + 1),
            label
          );
        return buttons.at(label);
      }

      Region2D ButtonRegion(int i) const  {
        i = (i-1) * button_width;
        return grid.CalculateGridRegion(
            2+i,   1,
            2+i+1, 2
          );
      }

      virtual void OnPointerEvent(PointerEvent event) override {
        for (auto& [label, button] : buttons) {
          if (button.Bounds().Encloses(event.Position())) {
            button.OnPointerEvent(event);
            return;
          }
        }
      };

      virtual Region2D Bounds() const override {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const override {
        int x = 1,
            y = 1,
            w = grid.MaxColumns() - 1
          ;

        int westcap_width = 1,
            eastcap_width = 1
          ;

        int filler_start = westcap_width + 1;
        int filler_end = w;

        Region2D   left_cap = grid.CalculateGridRegion(  x  , y,   x  , y+1);
        Region2D  right_cap = grid.CalculateGridRegion(w+x  , y, w+x  , y+1);
        Region2D centre_bar = grid.CalculateGridRegion(  x+1, y, w+x-1, y+1);

          left_cap.Fill    (renderer, Colours().light);
         right_cap.Bullnose(renderer, Compass::EAST, Colours().light);
        centre_bar.Fill    (renderer, Colours().background);

        if (buttons.size() > 0) {
          for (auto const& [label, button] : buttons) {
            button.Draw(renderer);
            filler_start += button_width;
          }
        }

        if (text.length() > 0) {
          std::string padded = std::string(" ") + text + " ";
          RenderedText headertext = window.HeaderFont().RenderText(Colours().active, padded);
          Region2D headerregion = centre_bar.Align(Compass::EAST, headertext.Size());

          int near = grid.PositionColumnIndex(headerregion.Near());
          int  far = grid.PositionColumnIndex(headerregion.Far());
          filler_end -= (far - near) + 1;

          int col = grid.PositionColumnIndex(headerregion.Near());
          Region2D cell = grid.CalculateGridRegion(col, y, col, y+1);
          Region2D fillerregion{
              cell.Origin(),
              Position2D{
                  headerregion.SouthWestX(),
                  cell.FarY()
                }
            };
          fillerregion.Fill(renderer, Colours().light);

          headertext.Draw(renderer, Compass::EAST, centre_bar);
        }

        grid.CalculateGridRegion(
            filler_start, y,
            filler_end, y+1
          ).Fill(renderer, Colours().frame);
      }
  };

  class header_pair_bar : public drawable {
    protected:
      Grid grid;
      Window& window;
      std::string left{""};
      std::string right{""};

    public:
      header_pair_bar(Grid g, Window& w,
          std::string l, std::string r)
        : grid{g}, window{w}, left{l}, right{r}
      {};

      header_pair_bar(Grid g, Window& w, Compass a)
        : grid{g}, window{w}
      {};

      void Left(std::string newlabel) {
          left = newlabel;
      }

      void Right(std::string newlabel) {
          right = newlabel;
      }

      virtual ColourScheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(ColourScheme cs) {
        drawable::Colours(cs);
      }

      virtual Region2D Bounds() const override {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const override {
        Region2D left_cap = grid.CalculateGridRegion(
            1, 1,
            1, grid.MaxRows()
          );

        Region2D centre_bar = grid.CalculateGridRegion(
            2, 1,
            grid.MaxColumns() - 1, grid.MaxRows()
          );

        Region2D right_cap = grid.CalculateGridRegion(
            grid.MaxColumns(), 1,
            grid.MaxColumns(), grid.MaxRows()
          );

        std::string paddedleft = " " + left + " ";
        std::string paddedright = " " + right + " ";

        auto const& headerfont = window.HeaderFont();
        RenderedText  lefttext = headerfont.RenderText(
            Colours().active, paddedleft
          );
        RenderedText righttext = headerfont.RenderText(
            Colours().active, paddedright
          );

        Region2D lefttextregion = centre_bar.Align(
            Compass::WEST, lefttext.Size()
          );
        Region2D righttextregion = centre_bar.Align(
            Compass::EAST, righttext.Size()
          );

        Position2D leftlimit = lefttextregion.SouthEast();
        Position2D rightlimit = righttextregion.NorthWest();

        int left_text_end_col_index = grid.PositionColumnIndex(
            lefttextregion.East()
          );
        int right_text_end_col_index = grid.PositionColumnIndex(
            righttextregion.West()
          );

        Region2D drawcentrebar = grid.CalculateGridRegion(
            left_text_end_col_index + 1, 1,
            right_text_end_col_index - 1, grid.MaxRows()
          );

        Region2D left_text_end_cell =
          grid.CalculateGridRegion(
              left_text_end_col_index, 1,
              left_text_end_col_index, grid.MaxRows()
            );

        Region2D right_text_end_cell =
          grid.CalculateGridRegion(
              right_text_end_col_index, 1,
              right_text_end_col_index, grid.MaxRows()
            );

        Region2D left_text_filler{
            Position2D(leftlimit.x, left_text_end_cell.NorthWestY()),
            left_text_end_cell.SouthEast()
          };

        Region2D right_text_filler{
            right_text_end_cell.NorthWest(),
            Position2D(rightlimit.x, right_text_end_cell.SouthEastY())
          };

        if (right_text_filler.W() >= grid.Gutter().x) {
          right_text_filler.Fill(renderer, Colours().light);
        }

        if (left_text_filler.W() >= grid.Gutter().x) {
          left_text_filler.Fill(renderer, Colours().light);
        }

        drawcentrebar.Fill(renderer, Colours().frame);
        left_cap.Bullnose(renderer, Compass::WEST, Colours().light);
        right_cap.Bullnose(renderer, Compass::EAST, Colours().light);

        lefttext.Draw(renderer, Compass::WEST, centre_bar);
        righttext.Draw(renderer, Compass::EAST, centre_bar);
      }
  };


}