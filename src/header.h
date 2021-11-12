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
      : grid{g}, window{w}, alignment{a}, text{t}
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
      if (text.length() == 0) {
        grid.bounds.Fill(renderer, Colours().frame);
        return;
      }

      std::string padded = std::string(" ") + text + " ";

      RenderedText headertext = window.HeaderFont().RenderText(Colours().active, padded);
      headertext.Draw(renderer, alignment, grid.bounds);
    }
};

class EastHeaderBar : public Drawable {
  protected:
    Grid grid;
    Window& window;
    std::string text{""};
    std::list<Button> buttons;
    int button_width{2};

  public:
    EastHeaderBar(Grid g, Window& w, Compass a, std::string t)
      : grid{g}, window{w}, text{t}
    {};

    EastHeaderBar(Grid g, Window& w, Compass a)
      : grid{g}, window{w}
    {};

    void Label(std::string newlabel) {
        text = newlabel;
    }
    virtual ColourScheme Colours() const {
      return Drawable::Colours();
    }

    virtual void Colours(ColourScheme cs) {
      for (auto& button : buttons) {
        button.Colours(cs);
      }
      Drawable::Colours(cs);
    }

    Button& AddButton(std::string label) {
      buttons.emplace_back(
          window,
          ButtonRegion(buttons.size() + 1),
          label
        );
        return buttons.back();
    }

    Region2D ButtonRegion(int i) const  {
      i = (i-1) * button_width;
      return grid.CalculateGridRegion(
          2+i,   1,
          2+i+1, 2
        );
    }

    virtual void OnPointerEvent(PointerEvent event) override {
      for (auto& button : buttons) {
        if (button.Bounds().Encloses(event.Position())) {
          button.OnPointerEvent(event);
          return;
        }
      }
    };

    virtual Region2D Bounds() const override {
      return grid.bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      int x = 1,
          y = 1,
          w = grid.MaxColumns() - 1
        ;

      int westcap_width = 1,
          eastcap_width = 1
        ;

      int filler_start = westcap_width + 1;
      int filler_end = w;

      Region2D   left_cap = grid.CalculateGridRegion(  x  , y,   x  , y+1);
      Region2D  right_cap = grid.CalculateGridRegion(w+x  , y, w+x  , y+1);
      Region2D centre_bar = grid.CalculateGridRegion(  x+1, y, w+x-1, y+1);

        left_cap.Fill    (renderer, Colours().light);
       right_cap.Bullnose(renderer, Compass::EAST, Colours().light);
      centre_bar.Fill    (renderer, Colours().background);

      if (buttons.size() > 0) {
        for (auto const& button : buttons) {
          button.Draw(renderer);
          filler_start += button_width;
        }
      }

      if (text.length() > 0) {
        std::string padded = std::string(" ") + text + " ";
        RenderedText headertext = window.HeaderFont().RenderText(Colours().active, padded);
        Region2D headerregion = centre_bar.Align(Compass::EAST, headertext.Size());

        int near = grid.PositionColumnIndex(headerregion.Near());
        int  far = grid.PositionColumnIndex(headerregion.Far());
        filler_end -= (far - near) + 1;

        int col = grid.PositionColumnIndex(headerregion.Near());
        Region2D cell = grid.CalculateGridRegion(col, y, col, y+1);
        Region2D fillerregion{
            cell.Origin(),
            Position2D{
                headerregion.SouthWestX(),
                cell.FarY()
              }
          };
        fillerregion.Fill(renderer, Colours().light);

        headertext.Draw(renderer, Compass::EAST, centre_bar);
      }

      grid.CalculateGridRegion(
          filler_start, y,
          filler_end, y+1
        ).Fill(renderer, Colours().frame);
    }
};

