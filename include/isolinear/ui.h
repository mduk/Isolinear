#pragma once

#include <list>
#include <map>
#include <string>

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <miso.h>

#include "colours.h"
#include "display.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"


using isolinear::compass;
using isolinear::geometry::Region2D;
namespace display = isolinear::display;


extern bool drawdebug;


namespace isolinear::ui {

  // Widgets

  class horizontal_rule : public drawable {

    protected:
      display::window& window;
      isolinear::grid grid;

    public:
      horizontal_rule(display::window& w, isolinear::grid g)
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
      display::window& window;
      bool enabled = true;
      bool active = false;
      std::string label;

    public:
      miso::signal<> signal_press;
      Region2D bounds;

      button( display::window& w, isolinear::grid g, std::string l)
        : button(w, g.bounds, l)
      {}

      button(
          display::window& w,
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
            compass::southeast,
            std::string{" "} + label + " "
          );
      }


      void OnPointerEvent(PointerEvent event) {
        emit signal_press();
      }
  };


  class button_bar : public drawable {
    protected:
      isolinear::grid grid;
      display::window& window;
      std::map<std::string, isolinear::ui::button> buttons;
      vector button_size{2,2};

    public:
      button_bar(display::window& w, isolinear::grid g)
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
      horizontal_button_bar(display::window& w, isolinear::grid g) : button_bar(w, g) {}

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
      vertical_button_bar(display::window& w, isolinear::grid g) : button_bar(w, g) {}

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
      isolinear::grid grid;
      display::window& window;
      compass alignment = compass::centre;
      std::string text{""};

    public:
      header_basic(isolinear::grid g, display::window& w, compass a)
        : header_basic(g, w, a, "")
      {}

      header_basic(isolinear::grid g, display::window& w, compass a, std::string t)
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
      isolinear::grid grid;
      display::window& window;
      std::string text{""};
      std::map<std::string, isolinear::ui::button> buttons;
      int button_width{2};

    public:
      header_east_bar(display::window& w, isolinear::grid g, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      header_east_bar(isolinear::grid g, display::window& w, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      header_east_bar(isolinear::grid g, display::window& w, compass a, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      header_east_bar(isolinear::grid g, display::window& w, compass a)
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
         right_cap.Bullnose(renderer, compass::east, Colours().light);
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
          Region2D headerregion = centre_bar.Align(compass::east, headertext.Size());

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

          headertext.Draw(renderer, compass::east, centre_bar);
        }

        grid.CalculateGridRegion(
            filler_start, y,
            filler_end, y+1
          ).Fill(renderer, Colours().frame);
      }
  };

  class header_pair_bar : public drawable {
    protected:
      isolinear::grid grid;
      display::window& window;
      std::string left{""};
      std::string right{""};

    public:
      header_pair_bar(isolinear::grid g, display::window& w,
          std::string l, std::string r)
        : grid{g}, window{w}, left{l}, right{r}
      {};

      header_pair_bar(isolinear::grid g, display::window& w, compass a)
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
            compass::west, lefttext.Size()
          );
        Region2D righttextregion = centre_bar.Align(
            compass::east, righttext.Size()
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
        left_cap.Bullnose(renderer, compass::west, Colours().light);
        right_cap.Bullnose(renderer, compass::east, Colours().light);

        lefttext.Draw(renderer, compass::west, centre_bar);
        righttext.Draw(renderer, compass::east, centre_bar);
      }
  };


  class label : public drawable {
    protected:
      display::window& window;
      Region2D bounds;
      std::string label_text;

    public:
      label(display::window& w, Region2D b, std::string l)
        : window(w)
        , bounds(b)
        , label_text(l)
      {}

      label(display::window& w, isolinear::grid g, std::string l)
        : label(w, g.bounds, l)
      {}

    public:
      Region2D Bounds() const {
        return bounds;
      }

      void Draw(SDL_Renderer* renderer) const {
        window.LabelFont().RenderText(
            renderer,
            bounds,
            compass::west,
            std::string{" "} + label_text + " "
          );
      }

  };




  class Sweep : public drawable {
    protected:
      display::window& window;
      isolinear::grid grid;
      vector ports;
      int outer_radius;
      int inner_radius;
      compass alignment;
      compass opposite;

    public:
      Sweep(display::window& w, isolinear::grid g, vector p, int oradius, int iradius, compass ali)
        : window{w}, grid{g}, ports{p}, outer_radius{oradius}, inner_radius{iradius}, alignment{ali}
      {
        switch (alignment) {
          case compass::northeast: opposite = compass::southwest; break;
          case compass::southeast: opposite = compass::northwest; break;
          case compass::southwest: opposite = compass::northeast; break;
          case compass::northwest: opposite = compass::southeast; break;
        }
      }

      int VerticalPortSize() const {
        return ports.x;
      }

      int HorizontalPortSize() const {
        return ports.y;
      }

      virtual Region2D HorizontalPort() const = 0;
      virtual Region2D VerticalPort() const = 0;

      virtual Region2D InnerCornerRegion() const {
        return Region2D{
            HorizontalPort().Point(opposite),
            VerticalPort().Point(opposite)
          };
      }

      Region2D OuterRadiusRegion() const {
        return grid.bounds.Align(alignment, Size2D{outer_radius});
      }

      void DrawOuterRadius(SDL_Renderer* renderer) const {
        Region2D region = OuterRadiusRegion();
        region.Fill(renderer, Colours().background);
        region.QuadrantArc(renderer, alignment, Colours().frame);
      }

      Region2D Bounds() const override {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const override {
        Region2D icorner = InnerCornerRegion();
        Region2D iradius = icorner.Align(alignment, Size2D{inner_radius});

        grid.bounds.Fill(renderer, Colours().frame);
        icorner.Fill(renderer, Colours().background);

        iradius.Fill(renderer, Colours().frame);
        iradius.QuadrantArc(renderer, alignment, Colours().background);
        DrawOuterRadius(renderer);

        if (drawdebug) {
          HorizontalPort().Draw(renderer);
          VerticalPort().Draw(renderer);
          icorner.Draw(renderer);
        }
      }
  };

  class NorthEastSweep : public Sweep {
    public:
      NorthEastSweep(display::window& window, isolinear::grid grid,  vector ports, int oradius, int iradius)
        : Sweep{window, grid, ports, oradius, iradius, compass::northeast}
      {}

      Region2D HorizontalPort() const override {
        return grid.CalculateGridRegion(
            1, 1,
            1, ports.y
          );
      }

      Region2D VerticalPort() const override {
        return grid.CalculateGridRegion(
            grid.MaxColumns() - ports.x + 1, grid.MaxRows(),
                          grid.MaxColumns(), grid.MaxRows()
          );
      }

  };

  class SouthEastSweep : public Sweep {
    public:
      SouthEastSweep(display::window& window, isolinear::grid grid,  vector ports, int oradius, int iradius)
        : Sweep{window, grid, ports, oradius, iradius, compass::southeast}
      {}

      Region2D HorizontalPort() const override {
        return grid.CalculateGridRegion(
            1, grid.MaxRows() - ports.y + 1,
            1, grid.MaxRows()
          );
      }

      Region2D VerticalPort() const override {
        return grid.CalculateGridRegion(
            grid.MaxColumns() - ports.x + 1, 1,
            grid.MaxColumns(), 1
          );
      }

  };

  class SouthWestSweep : public Sweep {
    public:
      SouthWestSweep(display::window& window, isolinear::grid grid,  vector ports, int oradius, int iradius)
        : Sweep{window, grid, ports, oradius, iradius, compass::southwest}
      {}

      Region2D VerticalPort() const override {
        return grid.CalculateGridRegion(
            1, 1,
            ports.x, 1
          );
      }

      Region2D HorizontalPort() const override {
        return grid.CalculateGridRegion(
            grid.MaxColumns(), grid.MaxRows() - ports.y + 1,
                          grid.MaxColumns(), grid.MaxRows()
          );
      }

  };

  class NorthWestSweep : public Sweep {
    public:
      NorthWestSweep(display::window& window, isolinear::grid grid,  vector ports, int oradius, int iradius)
        : Sweep{window, grid, ports, oradius, iradius, compass::northwest}
      {}

      Region2D HorizontalPort() const override {
        return grid.CalculateGridRegion(
            grid.MaxColumns(), 1,
            grid.MaxColumns(), ports.y
          );
      }

      Region2D VerticalPort() const override {
        return grid.CalculateGridRegion(
            1, grid.MaxRows(),
            ports.x, grid.MaxRows()
          );
      }


  };


  class horizontal_progress_bar : public drawable {
    protected:
      isolinear::grid grid;
      unsigned max = 100;
      unsigned val = 50;
      unsigned gutter = 6;

      bool draw_stripes = true;
      bool draw_tail = true;

      Colour bar_colour;
      Region2D bar_region;
      Size2D segment_size;
      unsigned remainder_px = 0;
      unsigned n_segments = 0;


    public:
      miso::signal<> signal_valuechanged;

      horizontal_progress_bar(isolinear::grid _g)
        : grid{_g}
        , bar_region(
            Position2D(
              grid.bounds.NearX() + (gutter * 2),
              grid.bounds.NearY() + (gutter * 2)
            ),
            Position2D(
              grid.bounds.FarX() - (gutter * 2),
              grid.bounds.FarY() - (gutter * 2)
            )
          )
        , segment_size{ (int) gutter, bar_region.H() }
        , n_segments{ (unsigned) (bar_region.W() / segment_size.x) }
        , remainder_px{ bar_region.W() % segment_size.x }
      { };

      unsigned Max() {
        return max;
      }

      void Max(unsigned m) {
        if (m > val) {
          Val(m);
        }

        max = m;
      }

      unsigned Val() {
        return val;
      }

      void Val(unsigned v) {
        if (v == val) {
          return;
        }

        if (v > max) {
          val = max;
        }
        else {
          val = v;
        }

        emit signal_valuechanged();
      }

      void Inc(unsigned v) {
        if (val + v > max) {
          Val(max);
        }
        else {
          Val(val + v);
        }
      }

      void Dec(unsigned v) {
        if (v > val) {
          Val(0);
        }
        else {
          Val(val - v);
        }
      }

      unsigned Segments() const {
        return n_segments;
      }

      unsigned FilledSegments() const {
        return (val * n_segments) / max;
      }

      bool DrawTail() const {
        return draw_tail;
      }
      void DrawTail(bool v) {
        draw_tail = v;
      }

      bool DrawStripes() const {
        return draw_stripes;
      }
      void DrawStripes(bool v) {
        draw_stripes = v;
      }

      Region2D Bounds() const override {
        return grid.bounds;
      }

      void Draw(SDL_Renderer* renderer) const override {
        boxColor(renderer,
            grid.bounds.NearX(), grid.bounds.NearY(),
            grid.bounds.FarX(),  grid.bounds.FarY(),
            Colours().frame
          );
        boxColor(renderer,
            grid.bounds.NearX() + gutter, grid.bounds.NearY() + gutter,
            grid.bounds.FarX() - gutter,  grid.bounds.FarY() - gutter,
            Colours().background
          );

        if (draw_stripes) {
          for (int i=0; i<n_segments; i++) {
            Region2D region{
                Position2D{ bar_region.Near().x + (segment_size.x * i), bar_region.Near().y },
                segment_size
              };

            if (i % 2 == 0) {
              region.Fill(renderer, Colours().background);
            }
            else {
              region.Fill(renderer, Colours().light_alternate);
            }
          }
        }

        Colour bar_colour = Colours().active;

        Region2D filled_region{
            Position2D{ bar_region.Near().x + (segment_size.x * FilledSegments()), bar_region.Near().y },
            segment_size
          };
        filled_region.Fill(renderer, bar_colour);

        if (draw_tail) {
          for (int i=0; i<FilledSegments(); i++) {
            Region2D region{
                Position2D{ bar_region.Near().x + (segment_size.x * i), bar_region.Near().y },
                segment_size
              };
            region.Fill(renderer, bar_colour);
          }
        }
      }
  };

}
