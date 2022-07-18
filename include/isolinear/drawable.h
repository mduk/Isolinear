#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "theme.h"
#include "pointerevent.h"
#include "grid.h"



namespace isolinear::ui {


  using isolinear::geometry::position;
  using isolinear::geometry::region;


  class drawable {
    protected:
      region m_bounds;
      theme::colour_scheme m_colours;
      std::list<drawable*> m_children;
      bool m_pointer_within{false};

    public:
      virtual region bounds() const = 0;

      virtual void draw(SDL_Renderer* renderer) const {
        for (auto& child : m_children) {
          child->draw(renderer);
        }
      }

      virtual bool pointer_is_hovering() const {
          return m_pointer_within;
      }

      virtual void RegisterChild(drawable* child) {
        m_children.push_back(child);
        child->colours(colours());
      }

      virtual void on_pointer_event(pointer::event event) {
        m_pointer_within = bounds().encloses(event.Position());

        for (auto& child : m_children) {
          child->on_pointer_event(event);
        }
      }

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



  template <class T>
  class drawable_list : public std::list<T>,
                        public isolinear::ui::drawable {

    protected:
      isolinear::grid grid;

    public:
      drawable_list(isolinear::grid g)
        : grid(g)
      {}

    protected:
      virtual isolinear::grid grid_for_index(int index) = 0;

    public:
      isolinear::grid next_grid() {
        return grid_for_index(std::list<T>::size() + 1);
      }

      isolinear::geometry::region bounds() const {
        return grid.bounds();
      }

      virtual void draw(SDL_Renderer* renderer) const {
        for (auto& elem : *this) {
          elem.draw(renderer);
        }
      }

      virtual void colours(theme::colour_scheme cs) {
        drawable::colours(cs);
        for (auto& elem : *this) {
          elem.colours(cs);
        }
      }

      virtual void on_pointer_event(pointer::event event) {
        position p = event.Position();

        for (auto& elem : *this) {
          if (elem.bounds().encloses(p)) {
            elem.on_pointer_event(event);
          }
        }
      }

  };



}
