#pragma once

#include <list>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "theme.h"
#include "event.h"
#include "layout.h"



namespace isolinear::ui {


  using isolinear::geometry::position;
  using isolinear::geometry::region;


  class control {
    protected:
      layout::grid m_grid;
      theme::colour_scheme m_colours;
      std::list<control*> m_children;
      bool m_mouse_within_bounds{false};

    public:
      control(layout::grid g)
        : m_grid(g) {}

      virtual region bounds() const {
        return m_grid.bounds();
      }

      virtual void draw(SDL_Renderer* renderer) const {
        for (auto& child : m_children) {
          child->draw(renderer);
        }
      }

      virtual bool pointer_is_hovering() const {
          return m_mouse_within_bounds;
      }

      void register_child(control* child) {
        m_children.push_back(child);
        child->colours(colours());
      }

      virtual void on_pointer_event(event::pointer event) {
        bool within_bounds = bounds().encloses(event.position());

        if (within_bounds && !m_mouse_within_bounds) {
          on_mouse_enter(event);
        }

        if (!within_bounds && m_mouse_within_bounds) {
          on_mouse_leave(event);
        }

        m_mouse_within_bounds = within_bounds;

        if (m_mouse_within_bounds) {
          if (event.is_mouse_down()) on_mouse_down(event);
          if (event.is_mouse_up()) on_mouse_up(event);
        }

        for (auto& child : m_children) {
          child->on_pointer_event(event);
        }
      }

      virtual void on_keyboard_event(event::keyboard event) {
        for (auto& child : m_children) {
          child->on_keyboard_event(event);
        }
      }

      virtual void on_mouse_down(event::pointer event) { }
      virtual void on_mouse_up(event::pointer event) { }
      virtual void on_mouse_enter(event::pointer event) { }
      virtual void on_mouse_leave(event::pointer event) { }

      virtual theme::colour_scheme colours() const {
        return m_colours;
      }

      virtual void colours(theme::colour_scheme cs) {
        m_colours = cs;
        for (auto& child : m_children) {
          child->colours(cs);
        }
      }
  };

}
