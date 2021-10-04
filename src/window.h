#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"
#include "colours.h"

class WindowAssignment {
  public:
    Region* region;
    Button* button;
    WindowAssignment(Region* _r, Button* _b) : region{_r}, button{_b} {}
};

class Window {
  public:
    Region* where;
    int rowh;
    int gutter;

    Window();

    void Draw(SDL_Renderer* renderer);
    void Add(Drawable* drawable);
    void OnMouseButtonDown(SDL_MouseButtonEvent* event);

  protected:
    std::list<Drawable*> drawables;
};

