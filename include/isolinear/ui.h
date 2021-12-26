#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "window.h"
#include "grid.h"
#include "colours.h"
#include "pointerevent.h"

namespace isolinear::ui {

  // Abstracts

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

      virtual void OnPointerEvent(PointerEvent event) {
        Region2D   b = Bounds();
        Position2D p = event.Position();

        printf("Region: %d,%d (%d,%d) %d,%d \n",
            b.NearX(), b.NearY(),
            b.W(),     b.H(),
            b.FarX(),  b.FarY()
          );
        printf(" Click: %d,%d (%d,%d local)\n",
            p.x, p.y,
            p.x - b.NearX(),
            p.y - b.NearY()
          );

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

}
