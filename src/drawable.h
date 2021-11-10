#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"
#include "pointerevent.h"

class Drawable {
  protected:
    Region2D bounds;
    ColourScheme colours;
    std::list<Drawable*> children;

  public:
    virtual Region2D Bounds() const = 0;

    virtual void Draw(SDL_Renderer* renderer) const {
      for (auto& child : children) {
        child->Draw(renderer);
      }
    }

    virtual void RegisterChild(Drawable* child) {
      children.push_back(child);
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

