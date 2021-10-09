#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"
#include "geometry.h"

using namespace std;

class Drawable {
  public:
    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent&
      ) = 0;

    virtual Region Bounds() = 0;
};

class Window {
  public:
    Size size;

    Window(int, int);
    ~Window() {
      for (auto* drawable : drawables) {
        delete drawable;
      }
    }

    void Add(Drawable*);
    void Draw();
    void OnMouseButtonDown(SDL_MouseButtonEvent&);

  protected:
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    std::list<Drawable*> drawables;
};

class Button : public Drawable {
  public:
    Button(Region _b, ColourScheme _cs)
      : bounds{_b}, colours{_cs}
    {};

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    Region Bounds();

  protected:
    Region bounds;
    ColourScheme colours;
    bool active = false;
};

class Elbo : public Drawable {
  public:
    Elbo(Region _b, ColourScheme _cs, Size _s)
      : bounds{_b}, colours{_cs}, sweep_size{_s},
        inner_radius{sweep_size.y}
    {};

    Region SweepRegion() const;
    Region HorizontalRegion() const;
    Region VerticalRegion() const;
    Region InnerRadiusRegion() const;
    Region ContainerRegion() const;

    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent&);
    Region Bounds() {
      return bounds;
    };

  protected:
    Region bounds;
    ColourScheme colours;
    Size sweep_size;
    int inner_radius;
};
