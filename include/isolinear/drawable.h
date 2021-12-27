#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"
#include "pointerevent.h"


using isolinear::geometry::Position2D;
using isolinear::geometry::Region2D;


namespace isolinear::ui {

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

}
