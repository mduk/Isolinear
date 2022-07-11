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

    public:
      virtual region Bounds() const = 0;

      virtual void Draw(SDL_Renderer* renderer) const {
        for (auto& child : m_children) {
          child->Draw(renderer);
        }
      }

      virtual void RegisterChild(drawable* child) {
        m_children.push_back(child);
        child->Colours(Colours());
      }

      virtual void OnPointerEvent(pointer::event event) {
        position p = event.Position();

        for (auto& child : m_children) {
          if (child->Bounds().encloses(p)) {
            child->OnPointerEvent(event);
          }
        }
      }

      virtual theme::colour_scheme Colours() const {
        return m_colours;
      }

      virtual void Colours(theme::colour_scheme cs) {
        m_colours = cs;
        for (auto& child : m_children) {
          child->Colours(cs);
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

      isolinear::geometry::region Bounds() const {
        return grid.bounds();
      }

      virtual void Draw(SDL_Renderer* renderer) const {
        for (auto& elem : *this) {
          elem.Draw(renderer);
        }
      }

      virtual void Colours(theme::colour_scheme cs) {
        drawable::Colours(cs);
        for (auto& elem : *this) {
          elem.Colours(cs);
        }
      }

      virtual void OnPointerEvent(pointer::event event) {
        position p = event.Position();

        for (auto& elem : *this) {
          if (elem.Bounds().encloses(p)) {
            elem.OnPointerEvent(event);
          }
        }
      }

  };



}
