#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"
#include "pointerevent.h"
#include "grid.h"



namespace isolinear::ui {


  using isolinear::geometry::Position2D;
  using isolinear::geometry::Region2D;


  class drawable {
    protected:
      Region2D bounds;
      ColourScheme colours;
      std::list<drawable*> children;

    public:
      virtual Region2D Bounds() const = 0;

      virtual void Draw(SDL_Renderer* renderer) const {
        for (auto& child : children) {
          child->Draw(renderer);
        }
      }

      virtual void RegisterChild(drawable* child) {
        children.push_back(child);
        child->Colours(Colours());
      }

      virtual void Update() {
        for (auto& child : children) {
          child->Update();
        }
      }

      virtual void OnPointerEvent(pointer::event event) {
        Position2D p = event.Position();

        for (auto& child : children) {
          if (child->Bounds().Encloses(p)) {
            child->OnPointerEvent(event);
          }
        }
      }

      virtual ColourScheme Colours() const {
        return colours;
      }

      virtual void Colours(ColourScheme cs) {
        colours = cs;
        for (auto& child : children) {
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

      isolinear::geometry::Region2D Bounds() const {
        return grid.bounds;
      }

      virtual void Draw(SDL_Renderer* renderer) const {
        for (auto& elem : *this) {
          elem.Draw(renderer);
        }
      }

      virtual void Colours(ColourScheme cs) {
        drawable::Colours(cs);
        for (auto& elem : *this) {
          elem.Colours(cs);
        }
      }

      virtual void Update() {
        for (auto& elem : *this) {
          elem.Colours(drawable::Colours());
          elem.Update();
        }
      }

      virtual void OnPointerEvent(pointer::event event) {
        Position2D p = event.Position();

        for (auto& elem : *this) {
          if (elem.Bounds().Encloses(p)) {
            elem.OnPointerEvent(event);
          }
        }
      }

  };



}
