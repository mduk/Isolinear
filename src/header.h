#pragma once

#include <string>
#include <list>

#include "geometry.h"
#include "window.h"
#include "button.h"

class Header : public Region2D {
  protected:
    Grid grid;
    Window& window;
    std::string text{""};
    std::list<Button> buttons;
    int button_width{2};

  public:
    Header(Grid g, Window& w, std::string t)
      : grid{g}, window{w}, text{t}
    {};

    Header(Grid g, Window& w)
      : grid{g}, window{w}
    {};

    virtual ColourScheme Colours() const {
      return Region2D::Colours();
    }

    virtual void Colours(ColourScheme cs) {
      for (auto& button : buttons) {
        button.Colours(cs);
      }
      Region2D::Colours(cs);
    }

    void AddButton(std::string label) {
      buttons.emplace_back(
          window,
          ButtonRegion(buttons.size() + 1),
          Colours(),
          label
        );
    }

    Region2D ButtonRegion(int i) const  {
      i = (i-1) * button_width;
      return grid.CalculateGridRegion(
          2+i,   1,
          2+i+1, 2
        );
    }

    virtual void OnMouseButtonDown(SDL_MouseButtonEvent& e) override {
      Position2D cursor{e};
      for (auto& button : buttons) {
        if (button.Encloses(cursor)) {
          button.OnMouseButtonDown(e);
          return;
        }
      }
    };

    SDL_Rect SdlRect() const override {
      return grid.bounds.SdlRect();
    }

    void Draw(SDL_Renderer* renderer) const override {
      int x = 1,
          y = 1,
          w = grid.MaxColumns() - 1,
          h = grid.MaxRows() - 2;

      Region2D  left_cap = grid.CalculateGridRegion(  x  , y,   x  , y+1);
      Region2D right_cap = grid.CalculateGridRegion(w+x  , y, w+x  , y+1);
      Region2D       bar = grid.CalculateGridRegion(  x+1, y, w+x-1, y+1);

      left_cap.Bullnose(renderer, Compass::WEST, Colours().light);
      right_cap.Bullnose(renderer, Compass::EAST, Colours().light);

      if (text.length() == 0) {
        bar.Fill(renderer, Colours().dark);
        return;
      }

      RenderedText headertext = window.HeaderFont().RenderText(Colours().active, text);
      Region2D headerregion = bar.Align(Compass::EAST, headertext.Size());

      int col = grid.PositionColumnIndex(headerregion.Near());
      Region2D cell = grid.CalculateGridRegion(col, y, col, y+1);
      Region2D fillerregion{
          cell.Origin(),
          Position2D{
              headerregion.SouthWestX(),
              cell.FarY()
            }
        };

      grid.CalculateGridRegion(
          x+1+(buttons.size()*button_width), y,
                                      col-1, y+1
        ).Fill(renderer, Colours().frame);

      for (auto const& button : buttons) {
        button.Draw(renderer);
      }

      fillerregion.Fill(renderer, Colours().light);

      headertext.Draw(renderer, Compass::EAST, bar);
    }
};
