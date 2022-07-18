#pragma once

#include <list>
#include <map>
#include <string>

#include <iostream>
#include <utility>
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
  using isolinear::geometry::region;


  // Widgets

  class horizontal_rule : public drawable {

    protected:
      [[maybe_unused]] display::window& m_window;
      isolinear::grid m_grid;

    public:
      horizontal_rule(display::window& w, isolinear::grid g)
        : m_window(w)
        , m_grid(std::move(g))
      {}

    public:
      [[nodiscard]] region bounds() const {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        auto bound_height = m_grid.bounds().H();
        auto offset_px = (bound_height - m_grid.gutter().y) / 2;

        auto hrule_near_x = m_grid.bounds().near_x();
        auto hrule_near_y = m_grid.bounds().near_y() + offset_px;

        auto hrule_far_x = m_grid.bounds().far_x();
        auto hrule_far_y = m_grid.bounds().far_y() - offset_px;

        region hrule(
            position(hrule_near_x, hrule_near_y),
            position(hrule_far_x,  hrule_far_y)
          );

        hrule.fill(renderer, colours().frame);
      }
  };


  class button : public drawable {
    protected:
      display::window& m_window;
      bool m_enabled = true;
      bool m_active = false;
      std::string m_label;

    public:
      miso::signal<> signal_press;
      region m_bounds;

      button( display::window& w, isolinear::grid g, std::string l)
        : button(w, g.bounds(), l)
      {}

      button(
          display::window& w,
          region b,
          std::string l
        ) :
          m_bounds{b},
          m_window{w},
          m_label{l}
      {}

      void enable() { m_enabled = true; }
      void disable() { m_enabled = false; }
      bool enabled() const { return m_enabled; }
      bool disabled() const { return !m_enabled; }

      void activate() { m_active = true; }
      void deactivate() { m_active = false; }
      void active(bool state) { m_active = state; }
      bool active() const { return m_active; }

      std::string label() { return m_label; }
      void label(std::string newlabel) { m_label = newlabel + " "; }

      virtual region bounds() const override {
        return m_bounds;
      }

      void draw(SDL_Renderer* renderer) const override {
        boxColor(renderer,
            m_bounds.near_x(), m_bounds.near_y(),
            m_bounds.far_x(),  m_bounds.far_y(),
            calculate_colour()
          );

        m_window.ButtonFont().RenderText(
            renderer,
            m_bounds,
            compass::southeast,
            std::string{" "} + m_label + " "
          );

        if (pointer_is_hovering()) {
            m_bounds.draw(renderer);
        }
      }


      void on_pointer_event(pointer::event event) override {
          drawable::on_pointer_event(event);
        emit signal_press();
      }

  protected:
      [[nodiscard]] theme::colour calculate_colour() const {
          if (disabled()) return colours().disabled;
          if (active()) return colours().active;
          if (pointer_is_hovering()) return colours().light;
          return colours().light_alternate;
      }
  };


  class button_bar : public drawable {
    protected:
      isolinear::grid m_grid;
      display::window& m_window;
      std::map<std::string, isolinear::ui::button> m_buttons;
      geometry::vector m_button_size{2,2};

    public:
      button_bar(display::window& w, isolinear::grid g)
        : m_window{w}, m_grid{g}
      {};

      virtual region calculate_button_region(int i) const = 0;
      virtual region calculate_bar_region() const = 0;

      virtual theme::colour_scheme colours() const {
        return drawable::colours();
      }

      virtual void colours(theme::colour_scheme cs) {
        for (auto& [label, button] : m_buttons) {
          button.colours(cs);
        }
        drawable::colours(cs);
      }

      virtual isolinear::ui::button& add_button(std::string label) {
        m_buttons.try_emplace(
                label,
                m_window,
                calculate_button_region(m_buttons.size() + 1),
                label
          );
        return m_buttons.at(label);
      }

      virtual isolinear::ui::button& get_button(std::string label) {
        return m_buttons.at(label);
      }

      virtual void deactivate_all() {
        for (auto& [label, button] : m_buttons) {
          button.deactivate();
        }
      }

      virtual void activate_one(std::string &label) {
        m_buttons.at(label).activate();
      }

      void on_pointer_event(pointer::event event) override {
        drawable::on_pointer_event(event);
        for (auto& [label, button] : m_buttons) {
            button.on_pointer_event(event);
        }
      };

      region bounds() const override {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        for (auto const& [label, button] : m_buttons) {
          button.draw(renderer);
        }

        region bar = calculate_bar_region();

        boxColor(renderer,
            bar.near_x(), bar.near_y(),
            bar.far_x(),  bar.far_y(),
            colours().frame
          );

        if (pointer_is_hovering()) {
            m_grid.draw(renderer);
        }
      }
  };


  class horizontal_button_bar : public button_bar {
    public:
      horizontal_button_bar(display::window& w, isolinear::grid g) : button_bar(w, g) {}

      region calculate_button_region(int i) const override {
        int near_col = m_button_size.x * (i-1) + 1;
        int near_row = 1;
        int  far_col = m_button_size.x * i;
        int  far_row = m_grid.max_rows();

        return m_grid.calculate_grid_region(
            near_col, near_row,
             far_col, far_row
          );
      }

      region calculate_bar_region() const override {
        int near_col = m_button_size.x * m_buttons.size() + 1;
        int near_row = 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_grid.max_rows();

        return m_grid.calculate_grid_region(
            near_col, near_row,
             far_col, far_row
          );
      }
  };


  class vertical_button_bar : public button_bar {
    public:
      vertical_button_bar(display::window& w, isolinear::grid g) : button_bar(w, g) {}

      region calculate_button_region(int i) const override {
        int near_col = 1;
        int near_row = m_button_size.y * (i-1) + 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_button_size.y * i;

        return m_grid.calculate_grid_region(
            near_col, near_row,
             far_col, far_row
          );
      }

      region calculate_bar_region() const override {
        int near_col = 1;
        int near_row = m_button_size.y * m_buttons.size() + 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_grid.max_rows();

        return m_grid.calculate_grid_region(
            near_col, near_row,
             far_col, far_row
          );
      }
  };


  class header_basic : public drawable {
    protected:
      isolinear::grid m_grid;
      display::window& m_window;
      compass m_alignment = compass::centre;
      std::string m_text{""};

    public:
      header_basic(isolinear::grid g, display::window& w, compass a)
        : header_basic(g, w, a, "")
      {}

      header_basic(isolinear::grid g, display::window& w, compass a, std::string t)
        : m_grid{g}, m_window{w}, m_alignment{a}, m_text{t}
      {}


      void label(std::string newlabel) {
          m_text = newlabel;
      }
      virtual theme::colour_scheme colours() const {
        return drawable::colours();
      }

      virtual void colours(theme::colour_scheme cs) {
        drawable::colours(cs);
      }

      virtual region bounds() const override {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        if (m_text.length() == 0) {
          m_grid.bounds().fill(renderer, colours().frame);
          return;
        }

        std::string padded = std::string(" ") + m_text + " ";

        text::rendered_text headertext = m_window.HeaderFont().RenderText(colours().active, padded);
        headertext.draw(renderer, m_alignment, m_grid.bounds());
      }
  };

  class header_east_bar : public drawable {
    protected:
      isolinear::grid m_grid;
      display::window& m_window;
      std::string m_text{""};
      std::map<std::string, isolinear::ui::button> m_buttons;
      int m_button_width{2};
      theme::colour m_left_cap_colour;
      theme::colour m_right_cap_colour;

    public:
      header_east_bar(display::window& w, isolinear::grid g, std::string t)
        : m_grid{g}, m_window{w}, m_text{t}
      {};

      header_east_bar(isolinear::grid g, display::window& w, std::string t)
        : m_grid{g}, m_window{w}, m_text{t}
      {};

      header_east_bar(isolinear::grid g, display::window& w, compass a, std::string t)
        : m_grid{g}, m_window{w}, m_text{t}
      {};

      header_east_bar(isolinear::grid g, display::window& w, compass a)
        : m_grid{g}, m_window{w}
      {};

      void label(std::string newlabel) {
          m_text = newlabel;
      }
      virtual std::string label() const {
          return m_text;
      }

      virtual theme::colour_scheme colours() const {
        return drawable::colours();
      }

      virtual void colours(theme::colour_scheme cs) {
        m_left_cap_colour = cs.light;
        m_right_cap_colour = cs.light;

        for (auto& [label, button] : m_buttons) {
          button.colours(cs);
        }

        drawable::colours(cs);
      }

      virtual theme::colour left_cap_colour() const {
        return m_left_cap_colour;
      }
      virtual theme::colour right_cap_colour() const {
        return m_right_cap_colour;
      }

      isolinear::ui::button& add_button(std::string label) {
        m_buttons.try_emplace(
                label,
                m_window,
                calculate_button_region(m_buttons.size() + 1),
                label
          );
        return m_buttons.at(label);
      }

      region calculate_button_region(int i) const  {
        i = (i-1) * m_button_width;
        return m_grid.calculate_grid_region(
            2+i,   1,
            2+i+1, 2
          );
      }

      void on_pointer_event(pointer::event event) override {
        drawable::on_pointer_event(event);
        for (auto& [label, button] : m_buttons) {
            button.on_pointer_event(event);
        }
      };

      region bounds() const override {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        int x = 1,
            y = 1,
            w = m_grid.max_columns() - 1
          ;

        int westcap_width = 1,
            eastcap_width = 1
          ;

        int filler_start = westcap_width + 1;
        int filler_end = w;

        region   left_cap = m_grid.calculate_grid_region(  x  , y,   x  , y+1);
        region  right_cap = m_grid.calculate_grid_region(w+x  , y, w+x  , y+1);
        region centre_bar = m_grid.calculate_grid_region(  x+1, y, w+x-1, y+1);

          left_cap.fill(renderer, left_cap_colour());
         right_cap.bullnose(renderer, compass::east, right_cap_colour());
        centre_bar.fill(renderer, colours().background);

        if (m_buttons.size() > 0) {
          for (auto const& [label, button] : m_buttons) {
            button.draw(renderer);
            filler_start += m_button_width;
          }
        }

        auto header_text = label();
        if (header_text.length() > 0) {
          std::string padded = std::string(" ") + header_text + " ";
          text::rendered_text headertext = m_window.HeaderFont().RenderText(colours().active, padded);
          region headerregion = centre_bar.align(compass::east, headertext.size());

          int near = m_grid.position_column_index(headerregion.Near());
          int  far = m_grid.position_column_index(headerregion.Far());
          filler_end -= (far - near) + 1;

          int col = m_grid.position_column_index(headerregion.Near());
          region cell = m_grid.calculate_grid_region(col, y, col, y+1);
          region fillerregion{
              cell.origin(),
              position{
                  headerregion.southwest_x(),
                  cell.far_y()
                }
            };
          fillerregion.fill(renderer, right_cap_colour());

          headertext.draw(renderer, compass::east, centre_bar);
        }

        m_grid.calculate_grid_region(
            filler_start, y,
            filler_end, y+1
          ).fill(renderer, colours().frame);
      }
  };

  class header_pair_bar : public drawable {
    protected:
      isolinear::grid m_grid;
      display::window& m_window;
      std::string m_left{""};
      std::string m_right{""};

    public:
      header_pair_bar(isolinear::grid g, display::window& w,
          std::string l, std::string r)
        : m_grid{g}, m_window{w}, m_left{l}, m_right{r}
      {};

      header_pair_bar(isolinear::grid g, display::window& w, compass a)
        : m_grid{g}, m_window{w}
      {};

      void Left(std::string newlabel) {
          m_left = newlabel;
      }

      void Right(std::string newlabel) {
          m_right = newlabel;
      }

      virtual theme::colour_scheme colours() const {
        return drawable::colours();
      }

      virtual void colours(theme::colour_scheme cs) {
        drawable::colours(cs);
      }

      virtual region bounds() const override {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        region left_cap = m_grid.calculate_grid_region(
            1, 1,
            1, m_grid.max_rows()
          );

        region centre_bar = m_grid.calculate_grid_region(
            2, 1,
            m_grid.max_columns() - 1, m_grid.max_rows()
          );

        region right_cap = m_grid.calculate_grid_region(
            m_grid.max_columns(), 1,
            m_grid.max_columns(), m_grid.max_rows()
          );

        std::string paddedleft = " " + m_left + " ";
        std::string paddedright = " " + m_right + " ";

        auto const& headerfont = m_window.HeaderFont();
        text::rendered_text  lefttext = headerfont.RenderText(
            colours().active, paddedleft
          );
        text::rendered_text righttext = headerfont.RenderText(
            colours().active, paddedright
          );

        region lefttextregion = centre_bar.align(
            compass::west, lefttext.size()
          );
        region righttextregion = centre_bar.align(
            compass::east, righttext.size()
          );

        position leftlimit = lefttextregion.southeast();
        position rightlimit = righttextregion.northwest();

        int left_text_end_col_index = m_grid.position_column_index(
            lefttextregion.east()
          );
        int right_text_end_col_index = m_grid.position_column_index(
            righttextregion.west()
          );

        region drawcentrebar = m_grid.calculate_grid_region(
            left_text_end_col_index + 1, 1,
            right_text_end_col_index - 1, m_grid.max_rows()
          );

        region left_text_end_cell =
          m_grid.calculate_grid_region(
              left_text_end_col_index, 1,
              left_text_end_col_index, m_grid.max_rows()
            );

        region right_text_end_cell =
          m_grid.calculate_grid_region(
              right_text_end_col_index, 1,
              right_text_end_col_index, m_grid.max_rows()
            );

        region left_text_filler{
            position(leftlimit.x, left_text_end_cell.northwest_y()),
            left_text_end_cell.southeast()
          };

        region right_text_filler{
            right_text_end_cell.northwest(),
            position(rightlimit.x, right_text_end_cell.southeast_y())
          };

        if (right_text_filler.W() >= m_grid.gutter().x) {
          right_text_filler.fill(renderer, colours().light);
        }

        if (left_text_filler.W() >= m_grid.gutter().x) {
          left_text_filler.fill(renderer, colours().light);
        }

        drawcentrebar.fill(renderer, colours().frame);
        left_cap.bullnose(renderer, compass::west, colours().light);
        right_cap.bullnose(renderer, compass::east, colours().light);

        lefttext.draw(renderer, compass::west, centre_bar);
        righttext.draw(renderer, compass::east, centre_bar);
      }
  };


  class label : public drawable {
    protected:
      display::window& m_window;
      region m_bounds;
      std::string m_text;

    public:
      label(display::window& w, region b, std::string l)
        : m_window(w)
        , m_bounds(b)
        , m_text(l)
      {}

      label(display::window& w, isolinear::grid g, std::string l)
        : label(w, g.bounds(), l)
      {}

    public:
      region bounds() const {
        return m_bounds;
      }

      void draw(SDL_Renderer* renderer) const {
        m_window.LabelFont().RenderText(
            renderer,
            m_bounds,
            compass::west,
            std::string{" "} + m_text + " "
          );
      }

  };




  class sweep : public drawable {
    protected:
      display::window& m_window;
      isolinear::grid m_grid;
      geometry::vector m_ports;
      int m_outer_radius;
      int m_inner_radius;
      compass m_alignment;
      compass m_opposite;

    public:
      sweep(display::window& w, isolinear::grid g,
          geometry::vector p,
          int oradius, int iradius, compass ali)
        : m_window{w}, m_grid{g}, m_ports{p}, m_outer_radius{oradius}, m_inner_radius{iradius}, m_alignment{ali}
      {
        switch (m_alignment) {
          case compass::northeast: m_opposite = compass::southwest; break;
          case compass::southeast: m_opposite = compass::northwest; break;
          case compass::southwest: m_opposite = compass::northeast; break;
          case compass::northwest: m_opposite = compass::southeast; break;
        }
      }

      virtual region HorizontalPort() const = 0;
      virtual region VerticalPort() const = 0;

      virtual region InnerCornerRegion() const {
        return region{
            HorizontalPort().point(m_opposite),
            VerticalPort().point(m_opposite)
          };
      }

      region OuterRadiusRegion() const {
        return m_grid.bounds().align(m_alignment, geometry::vector{m_outer_radius});
      }

      void DrawOuterRadius(SDL_Renderer* renderer) const {
        region region = OuterRadiusRegion();
        region.fill(renderer, colours().background);
        region.quadrant_arc(renderer, m_alignment, colours().frame);
      }

      region bounds() const override {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        region icorner = InnerCornerRegion();
        region iradius = icorner.align(m_alignment, geometry::vector{m_inner_radius});

        m_grid.bounds().fill(renderer, colours().frame);
        icorner.fill(renderer, colours().background);

        iradius.fill(renderer, colours().frame);
        iradius.quadrant_arc(renderer, m_alignment, colours().background);
        DrawOuterRadius(renderer);

        if (pointer_is_hovering()) {
          m_grid.draw(renderer);
          HorizontalPort().draw(renderer);
          VerticalPort().draw(renderer);
          icorner.draw(renderer);
        }
      }
  };

  class north_east_sweep : public sweep {
    public:
      north_east_sweep(display::window& m_window, isolinear::grid m_grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{m_window, m_grid, ports, oradius, iradius, compass::northeast}
      {}

      region HorizontalPort() const override {
        return m_grid.calculate_grid_region(
            1, 1,
            1, m_ports.y
          );
      }

      region VerticalPort() const override {
        return m_grid.calculate_grid_region(
            m_grid.max_columns() - m_ports.x + 1, m_grid.max_rows(),
                          m_grid.max_columns(), m_grid.max_rows()
          );
      }

  };

  class south_east_sweep : public sweep {
    public:
      south_east_sweep(display::window& m_window, isolinear::grid m_grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{m_window, m_grid, ports, oradius, iradius, compass::southeast}
      {}

      region HorizontalPort() const override {
        return m_grid.calculate_grid_region(
            1, m_grid.max_rows() - m_ports.y + 1,
            1, m_grid.max_rows()
          );
      }

      region VerticalPort() const override {
        return m_grid.calculate_grid_region(
            m_grid.max_columns() - m_ports.x + 1, 1,
            m_grid.max_columns(), 1
          );
      }

  };

  class south_west_sweep : public sweep {
    public:
      south_west_sweep(display::window& m_window, isolinear::grid m_grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{m_window, m_grid, ports, oradius, iradius, compass::southwest}
      {}

      region VerticalPort() const override {
        return m_grid.calculate_grid_region(
            1, 1,
            m_ports.x, 1
          );
      }

      region HorizontalPort() const override {
        return m_grid.calculate_grid_region(
            m_grid.max_columns(), m_grid.max_rows() - m_ports.y + 1,
                          m_grid.max_columns(), m_grid.max_rows()
          );
      }

  };

  class north_west_sweep : public sweep {
    public:
      north_west_sweep(display::window& m_window, isolinear::grid m_grid,  geometry::vector ports, int oradius, int iradius)
        : sweep{m_window, m_grid, ports, oradius, iradius, compass::northwest}
      {}

      region HorizontalPort() const override {
        return m_grid.calculate_grid_region(
            m_grid.max_columns(), 1,
            m_grid.max_columns(), m_ports.y
          );
      }

      region VerticalPort() const override {
        return m_grid.calculate_grid_region(
            1, m_grid.max_rows(),
            m_ports.x, m_grid.max_rows()
          );
      }


  };


  class horizontal_progress_bar : public drawable {
    protected:
      isolinear::grid m_grid;
      unsigned m_max = 100;
      unsigned m_val = 50;
      unsigned m_gutter = 6;

      bool m_draw_stripes = true;
      bool m_draw_tail = true;

      theme::colour m_bar_colour;
      region m_bar_region;
      geometry::vector m_segment_size;
      unsigned m_remainder_px = 0;
      unsigned m_n_segments = 0;


    public:
      miso::signal<> signal_valuechanged;

      horizontal_progress_bar(isolinear::grid _g)
        : m_grid{_g}
        , m_bar_region(
            position(
              m_grid.bounds().near_x() + (m_gutter * 2),
              m_grid.bounds().near_y() + (m_gutter * 2)
            ),
            position(
              m_grid.bounds().far_x() - (m_gutter * 2),
              m_grid.bounds().far_y() - (m_gutter * 2)
            )
          )
        , m_segment_size{ (int) m_gutter, m_bar_region.H() }
        , m_n_segments{ (unsigned) (m_bar_region.W() / m_segment_size.x) }
        , m_remainder_px{ static_cast<unsigned int>(m_bar_region.W() % m_segment_size.x) }
      { };

      unsigned Max() {
        return m_max;
      }

      void Max(unsigned m) {
        if (m > m_val) {
          Val(m);
        }

        m_max = m;
      }

      unsigned Val() {
        return m_val;
      }

      void Val(unsigned v) {
        if (v == m_val) {
          return;
        }

        if (v > m_max) {
          m_val = m_max;
        }
        else {
          m_val = v;
        }

        emit signal_valuechanged();
      }

      void Inc(unsigned v) {
        if (m_val + v > m_max) {
          Val(m_max);
        }
        else {
          Val(m_val + v);
        }
      }

      void Dec(unsigned v) {
        if (v > m_val) {
          Val(0);
        }
        else {
          Val(m_val - v);
        }
      }

      unsigned Segments() const {
        return m_n_segments;
      }

      unsigned FilledSegments() const {
        return (m_val * m_n_segments) / m_max;
      }

      bool DrawTail() const {
        return m_draw_tail;
      }
      void DrawTail(bool v) {
        m_draw_tail = v;
      }

      bool DrawStripes() const {
        return m_draw_stripes;
      }
      void DrawStripes(bool v) {
        m_draw_stripes = v;
      }

      region bounds() const override {
        return m_grid.bounds();
      }

      void draw(SDL_Renderer* renderer) const override {
        boxColor(renderer,
            m_grid.bounds().near_x(), m_grid.bounds().near_y(),
            m_grid.bounds().far_x(),  m_grid.bounds().far_y(),
            colours().frame
          );
        boxColor(renderer,
            m_grid.bounds().near_x() + m_gutter, m_grid.bounds().near_y() + m_gutter,
            m_grid.bounds().far_x() - m_gutter,  m_grid.bounds().far_y() - m_gutter,
            colours().background
          );

        if (m_draw_stripes) {
          for (int i=0; i<m_n_segments; i++) {
            region region{
                position{ m_bar_region.Near().x + (m_segment_size.x * i), m_bar_region.Near().y },
                m_segment_size
              };

            if (i % 2 == 0) {
              region.fill(renderer, colours().background);
            }
            else {
              region.fill(renderer, colours().light_alternate);
            }
          }
        }

        theme::colour m_bar_colour = colours().active;

        region filled_region{
            position{ static_cast<int>(m_bar_region.Near().x + (m_segment_size.x * FilledSegments())), m_bar_region.Near().y },
            m_segment_size
          };
        filled_region.fill(renderer, m_bar_colour);

        if (m_draw_tail) {
          for (int i=0; i<FilledSegments(); i++) {
            region region{
                position{ m_bar_region.Near().x + (m_segment_size.x * i), m_bar_region.Near().y },
                m_segment_size
              };
            region.fill(renderer, m_bar_colour);
          }
        }
      }
  };


  template <class T, int H>
  class layout_vertical : public isolinear::ui::drawable_list<T> {

    public:
      explicit layout_vertical(isolinear::grid g)
        : isolinear::ui::drawable_list<T>(g)
      {}

    protected:
      isolinear::grid grid_for_index(int index) override {
        int far_row = index * H;
        return isolinear::ui::drawable_list<T>::grid.rows(far_row-(H-1), far_row);
      }
  };

}
