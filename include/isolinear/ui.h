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

#include "theme.h"
#include "display.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"



extern bool drawdebug;


namespace isolinear::ui {


  using isolinear::compass;
  using isolinear::geometry::Region2D;


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

        auto hrule_near_x = grid.bounds.near_x();
        auto hrule_near_y = grid.bounds.near_y() + offset_px;

        auto hrule_far_x = grid.bounds.far_x();
        auto hrule_far_y = grid.bounds.far_y() - offset_px;

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
        theme::colour drawcolour = enabled == true
                          ? active == true
                            ? Colours().active
                            : Colours().light_alternate
                          : Colours().disabled;


        boxColor(renderer,
            bounds.near_x(), bounds.near_y(),
            bounds.far_x(),  bounds.far_y(),
            drawcolour
          );

        window.ButtonFont().RenderText(
            renderer,
            bounds,
            compass::southeast,
            std::string{" "} + label + " "
          );
      }


      void OnPointerEvent(pointer::event event) {
        emit signal_press();
      }
  };


  class button_bar : public drawable {
    protected:
      isolinear::grid grid;
      display::window& window;
      std::map<std::string, isolinear::ui::button> buttons;
      geometry::vector button_size{2,2};

    public:
      button_bar(display::window& w, isolinear::grid g)
        : window{w}, grid{g}
      {};

      virtual Region2D ButtonRegion(int i) const = 0;
      virtual Region2D BarRegion() const = 0;

      virtual theme::colour_scheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(theme::colour_scheme cs) {
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

      virtual void OnPointerEvent(pointer::event event) override {
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
            bar.near_x(), bar.near_y(),
            bar.far_x(),  bar.far_y(),
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
      virtual theme::colour_scheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(theme::colour_scheme cs) {
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

        text::rendered_text headertext = window.HeaderFont().RenderText(Colours().active, padded);
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
      theme::colour left_cap_colour;
      theme::colour right_cap_colour;

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
      virtual std::string Label() const {
          return text;
      }

      virtual theme::colour_scheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(theme::colour_scheme cs) {
        left_cap_colour = cs.light;
        right_cap_colour = cs.light;

        for (auto& [label, button] : buttons) {
          button.Colours(cs);
        }

        drawable::Colours(cs);
      }

      virtual theme::colour LeftCapColour() const {
        return left_cap_colour;
      }
      virtual theme::colour RightCapColour() const {
        return right_cap_colour;
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

      virtual void OnPointerEvent(pointer::event event) override {
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

          left_cap.Fill    (renderer, LeftCapColour());
         right_cap.Bullnose(renderer, compass::east, RightCapColour());
        centre_bar.Fill    (renderer, Colours().background);

        if (buttons.size() > 0) {
          for (auto const& [label, button] : buttons) {
            button.Draw(renderer);
            filler_start += button_width;
          }
        }

        auto header_text = Label();
        if (header_text.length() > 0) {
          std::string padded = std::string(" ") + header_text + " ";
          text::rendered_text headertext = window.HeaderFont().RenderText(Colours().active, padded);
          Region2D headerregion = centre_bar.Align(compass::east, headertext.Size());

          int near = grid.PositionColumnIndex(headerregion.Near());
          int  far = grid.PositionColumnIndex(headerregion.Far());
          filler_end -= (far - near) + 1;

          int col = grid.PositionColumnIndex(headerregion.Near());
          Region2D cell = grid.CalculateGridRegion(col, y, col, y+1);
          Region2D fillerregion{
              cell.Origin(),
              Position2D{
                  headerregion.southwest_x(),
                  cell.far_y()
                }
            };
          fillerregion.Fill(renderer, RightCapColour());

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

      virtual theme::colour_scheme Colours() const {
        return drawable::Colours();
      }

      virtual void Colours(theme::colour_scheme cs) {
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
        text::rendered_text  lefttext = headerfont.RenderText(
            Colours().active, paddedleft
          );
        text::rendered_text righttext = headerfont.RenderText(
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
            Position2D(leftlimit.x, left_text_end_cell.northwest_y()),
            left_text_end_cell.SouthEast()
          };

        Region2D right_text_filler{
            right_text_end_cell.NorthWest(),
            Position2D(rightlimit.x, right_text_end_cell.southeast_y())
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




  class sweep : public drawable {
    protected:
      display::window& window;
      isolinear::grid grid;
      geometry::vector ports;
      int outer_radius;
      int inner_radius;
      compass alignment;
      compass opposite;

    public:
      sweep(display::window& w, isolinear::grid g, geometry::vector p, int oradius, int iradius, compass ali)
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
        return grid.bounds.Align(alignment, geometry::vector{outer_radius});
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
        Region2D iradius = icorner.Align(alignment, geometry::vector{inner_radius});

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

  class north_east_sweep : public sweep {
    public:
      north_east_sweep(display::window& window, isolinear::grid grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{window, grid, ports, oradius, iradius, compass::northeast}
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

  class south_east_sweep : public sweep {
    public:
      south_east_sweep(display::window& window, isolinear::grid grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{window, grid, ports, oradius, iradius, compass::southeast}
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

  class south_west_sweep : public sweep {
    public:
      south_west_sweep(display::window& window, isolinear::grid grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{window, grid, ports, oradius, iradius, compass::southwest}
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

  class north_west_sweep : public sweep {
    public:
      north_west_sweep(display::window& window, isolinear::grid grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{window, grid, ports, oradius, iradius, compass::northwest}
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

      theme::colour bar_colour;
      Region2D bar_region;
      geometry::vector segment_size;
      unsigned remainder_px = 0;
      unsigned n_segments = 0;


    public:
      miso::signal<> signal_valuechanged;

      horizontal_progress_bar(isolinear::grid _g)
        : grid{_g}
        , bar_region(
            Position2D(
              grid.bounds.near_x() + (gutter * 2),
              grid.bounds.near_y() + (gutter * 2)
            ),
            Position2D(
              grid.bounds.far_x() - (gutter * 2),
              grid.bounds.far_y() - (gutter * 2)
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
            grid.bounds.near_x(), grid.bounds.near_y(),
            grid.bounds.far_x(),  grid.bounds.far_y(),
            Colours().frame
          );
        boxColor(renderer,
            grid.bounds.near_x() + gutter, grid.bounds.near_y() + gutter,
            grid.bounds.far_x() - gutter,  grid.bounds.far_y() - gutter,
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

        theme::colour bar_colour = Colours().active;

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


  class elbo : public drawable {

    protected:
      display::window& window;
      isolinear::grid& grid;

      int reach_weight{30};
      geometry::vector sweep_cells{4,2};
      geometry::vector gutter{10,10};
      std::string header_string{""};
      isolinear::compass header_alignment = isolinear::compass::centre;
      std::list<isolinear::ui::button> buttons{};

    public:
      elbo(
          display::window& w,
          isolinear::grid& g,
          std::string h,
          compass ha
        ) :
          window{w},
          grid{g},
          header_string{h},
          header_alignment{ha}
      {};

      virtual Region2D ContainerRegion() const = 0;
      virtual Region2D VerticalRegion() const = 0;
      virtual Region2D SweepRegion() const = 0;
      virtual Region2D SweepOuterRadiusRegion() const = 0;
      virtual Region2D SweepInnerCornerRegion() const = 0;
      virtual Region2D SweepInnerRadiusRegion() const = 0;
      virtual Region2D HorizontalRegion() const = 0;
      virtual Region2D ButtonRegion(int) const = 0;

      virtual Region2D ReachRegion() const = 0;
      virtual Region2D HeaderRegion() const = 0;

      virtual int SweepOuterRadius() const {
        return std::min(
            SweepRegion().W() / 2,
            VerticalRegion().W()
          );
      }

      virtual int SweepInnerRadius() const {
        return SweepRegion().H() / 2;
      }

      void OnPointerEvent(pointer::event event) override {
        Position2D cursor = event.Position();

        auto const container_region = ContainerRegion();
        if (container_region.Encloses(cursor)) {
          printf("Container: ");
          container_region.Print();
          return;
        }

        auto const vertical_region = VerticalRegion();
        if (vertical_region.Encloses(cursor)) {
          printf("Vertical: ");
          vertical_region.Print();
          return;
        }

        for (auto& button : buttons) {
          if (button.bounds.Encloses(cursor)) {
            button.OnPointerEvent(event);
            return;
          }
        }

        auto const sweep_region = SweepRegion();
        if (sweep_region.Encloses(cursor)) {
          printf("Sweep: ");
          sweep_region.Print();
          return;
        }

        auto const horizontal_region = HorizontalRegion();
        if (horizontal_region.Encloses(cursor)) {
          printf("Horizontal: ");
          horizontal_region.Print();

          auto const reach_region = ReachRegion();
          if (reach_region.Encloses(cursor)) {
            printf("Reach: ");
            reach_region.Print();
            return;
          }

          auto const header_region = HeaderRegion();
          if (header_region.Encloses(cursor)) {
            printf("Header: ");
            header_region.Print();
            return;
          }

          return;
        }

      };

      void AddButton(std::string label) {
        buttons.emplace_back(
            window,
            ButtonRegion(buttons.size() + 1),
            label
          );
      }

      virtual void Draw(SDL_Renderer* renderer) const override {
        DrawSweep(renderer);
        DrawReach(renderer);
        DrawVertical(renderer);
        DrawHeader(renderer);
        for (auto const& button : buttons) {
          button.Draw(renderer);
        }
      }

      virtual void DrawSweep(SDL_Renderer* renderer) const {
        Region2D sweep = SweepRegion();
        Region2D outer_radius = SweepOuterRadiusRegion();
        Region2D inner_corner = SweepInnerCornerRegion();
        Region2D inner_radius = SweepInnerRadiusRegion();

        boxColor(renderer,
            sweep.near_x(), sweep.near_y(),
            sweep.far_x(), sweep.far_y(),
            Colours().frame
          );

        boxColor(renderer,
            outer_radius.near_x(), outer_radius.near_y(),
            outer_radius.far_x(), outer_radius.far_y(),
            Colours().background
          );

        boxColor(renderer,
            inner_corner.near_x(), inner_corner.near_y(),
            inner_corner.far_x(), inner_corner.far_y(),
            Colours().background
          );

        boxColor(renderer,
            inner_radius.near_x(), inner_radius.near_y(),
            inner_radius.far_x(), inner_radius.far_y(),
            Colours().frame
          );

      };

      virtual void DrawReach(SDL_Renderer* renderer) const {
        Region2D reach = ReachRegion();
        boxColor(renderer,
            reach.near_x(), reach.near_y(),
            reach.far_x(), reach.far_y(),
            Colours().frame
          );
      }

      virtual void DrawHeader(SDL_Renderer* renderer) const {
        window.HeaderFont().RenderText(
            renderer,
            HeaderRegion(),
            header_alignment,
            header_string
          );
      }

      virtual void DrawVertical(SDL_Renderer* renderer) const {
        Region2D vertical = VerticalRegion();
        boxColor(renderer,
            vertical.near_x(), vertical.near_y(),
            vertical.far_x(), vertical.far_y(),
            Colours().frame
          );
      }
  };


  // // // // // // // // // // // // // // // // // // // //


  class northwest_elbo : public elbo {

    public:
      northwest_elbo(display::window& w, isolinear::grid& g, std::string h)
        : elbo(w, g, h, compass::northwest)
      {}

    protected:
      Region2D SweepRegion() const override {
        return grid.CalculateGridRegion(
          1,1,
          sweep_cells.x, sweep_cells.y
        );
      }

      Region2D SweepOuterRadiusRegion() const override {
        Region2D sweep = SweepRegion();

        return Region2D{
            sweep.Origin(),
            geometry::vector{SweepOuterRadius()}
          };
      }

      Region2D SweepInnerCornerRegion() const override {
        Region2D sweep = SweepRegion();

        return sweep.Align(
            compass::southeast,
            geometry::vector{
                sweep.far_x() - VerticalRegion().far_x(),
                HeaderRegion().H() + gutter.y
              }
          );
      }

      Region2D SweepInnerRadiusRegion() const override {
        return SweepInnerCornerRegion().Align(
            compass::northwest,
            geometry::vector{SweepInnerRadius()}
          );
      }

      Region2D HorizontalRegion() const override {
        return grid.CalculateGridRegion(
          sweep_cells.x + 1, 1,
          grid.MaxColumns(), 2
        );
      }

      Region2D ReachRegion() const override {
        Region2D horizontal = HorizontalRegion();

        return Region2D{
            horizontal.Origin(),
            geometry::vector{
              horizontal.W(),
              reach_weight
            }
          };
      }

      Region2D HeaderRegion() const override {
        Region2D horizontal = HorizontalRegion();

        return Region2D{
            Position2D{
              horizontal.Origin().x,
              horizontal.Origin().y
                + reach_weight
                + gutter.y
            },
            geometry::vector{
              horizontal.Size().x,
              horizontal.Size().y
                - reach_weight
                - gutter.y
            }
          };
      }

      Region2D VerticalRegion() const override {
        return grid.CalculateGridRegion(
            1, sweep_cells.y + 1 + buttons.size(),
            sweep_cells.x - 1, grid.MaxRows()
          );
      }

      Region2D ContainerRegion() const override {
        return grid.CalculateGridRegion(
            sweep_cells.x, sweep_cells.y + 1,
            grid.MaxColumns(), grid.MaxRows()
          );
      }

      Region2D ButtonRegion(int i) const override {
        return grid.CalculateGridRegion(
            1, sweep_cells.y +  i,
            sweep_cells.x - 1, sweep_cells.y  + i
          );
      }

      void DrawSweep(SDL_Renderer* renderer) const {
        Region2D outer_radius = SweepOuterRadiusRegion();
        Region2D inner_radius = SweepInnerRadiusRegion();

        elbo::DrawSweep(renderer);

        filledPieColor(renderer,
            outer_radius.far_x(), outer_radius.far_y(),
            outer_radius.H(),
            180, 270,
            Colours().frame
          );

        filledPieColor(renderer,
            inner_radius.far_x(), inner_radius.far_y(),
            inner_radius.H(),
            180, 270,
            Colours().background
          );
      }

  };



  class southwest_elbo : public elbo {

    public:
      southwest_elbo(display::window& w, isolinear::grid& g, std::string h)
        : elbo(w, g, h, compass::southwest)
      {}

    protected:
      Region2D SweepRegion() const override {
        return grid.CalculateGridRegion(
          1, grid.MaxRows() - sweep_cells.y + 1,
          sweep_cells.x, grid.MaxRows()
        );
      }

      Region2D SweepOuterRadiusRegion() const override {
        Region2D sweep = SweepRegion();

        return sweep.Align(
            compass::southwest,
            geometry::vector{SweepOuterRadius()}
          );
      }

      Region2D SweepInnerCornerRegion() const override {
        Region2D sweep = SweepRegion();
        Region2D vertical = VerticalRegion();
        Region2D header = HeaderRegion();

        return sweep.Align(
            compass::northeast,
            geometry::vector{
              sweep.far_x() - vertical.far_x(),
              header.H() + gutter.y
            }
          );
      }

      Region2D SweepInnerRadiusRegion() const override {
        Region2D sweep = SweepRegion();

        return SweepInnerCornerRegion().Align(
            compass::southwest,
            geometry::vector{SweepInnerRadius()}
          );
      }

      Region2D HorizontalRegion() const override {
        return grid.CalculateGridRegion(
          sweep_cells.x + 1, grid.MaxRows() - sweep_cells.y + 1,
          grid.MaxColumns(), grid.MaxRows()
        );
      }

      Region2D ReachRegion() const override {
        Region2D horizontal = HorizontalRegion();

        return Region2D{
            Position2D{
              horizontal.X(),
              horizontal.Y() + horizontal.H() - reach_weight
            },
            geometry::vector{
              horizontal.W(),
              reach_weight
            }
          };
      }

      Region2D HeaderRegion() const override {
        Region2D horizontal = HorizontalRegion();

        return Region2D{
            Position2D{
              horizontal.X(),
              horizontal.Y()
            },
            geometry::vector{
              horizontal.W(),
              horizontal.H() - reach_weight - gutter.y
            }
          };
      }

      Region2D VerticalRegion() const override {
        return grid.CalculateGridRegion(
            1,1,
            sweep_cells.x -1, grid.MaxRows() - sweep_cells.y - buttons.size()
          );
      }

      Region2D ContainerRegion() const override {
        return Region2D{ };
      }

      Region2D ButtonRegion(int i) const override {
        return grid.CalculateGridRegion(
            sweep_cells.x - 1, grid.MaxRows() - sweep_cells.y - buttons.size(),
            sweep_cells.x - 1, grid.MaxRows() - sweep_cells.y - buttons.size()
          );
      };

      void DrawSweep(SDL_Renderer* renderer) const override {
        Region2D outer_radius = SweepOuterRadiusRegion();
        Region2D inner_radius = SweepInnerRadiusRegion();

        elbo::DrawSweep(renderer);

        filledPieColor(renderer,
            outer_radius.northeast_x(),
            outer_radius.northeast_y(),
            outer_radius.H(),
            90, 180,
            Colours().frame
          );


        filledPieColor(renderer,
            inner_radius.northeast_x(),
            inner_radius.northeast_y(),
            inner_radius.H(),
            90, 180,
            Colours().background
          );
      }
  };

  template <class T, int H>
  class layout_vertical : public isolinear::ui::drawable_list<T> {

    public:
      layout_vertical(isolinear::grid g)
        : isolinear::ui::drawable_list<T>(g)
      {}

    protected:
      isolinear::grid grid_for_index(int index) override {
        int far_row = index * H;
        return isolinear::ui::drawable_list<T>::grid.Rows(far_row-(H-1), far_row);
      }
  };

}
