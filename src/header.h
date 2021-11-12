#pragma once

#include <string>
#include <list>

#include "miso.h"

#include "geometry.h"
#include "window.h"
#include "button.h"

class BasicHeader : public Drawable {
  protected:
    Grid grid;
    Window& window;
    Compass alignment{CENTRE};
    std::string text{""};

  public:
    BasicHeader(Grid g, Window& w, Compass a)
      : BasicHeader(g, w, a, "")
    {}

    BasicHeader(Grid g, Window& w, Compass a, std::string t)
      : grid{g}, window{w}, alignment{a}
    {}


    void Label(std::string newlabel) {
        text = newlabel;
    }
    virtual ColourScheme Colours() const {
      return Drawable::Colours();
    }

    virtual void Colours(ColourScheme cs) {
      Drawable::Colours(cs);
    }

    virtual Region2D Bounds() const override {
      return grid.bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      if (text.length() > 0) {
        RenderedText headertext = window.HeaderFont().RenderText(Colours().active, text);
        headertext.Draw(renderer, alignment, grid.bounds);
      }
    }
};

