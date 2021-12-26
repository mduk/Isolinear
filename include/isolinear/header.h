#pragma once

#include <string>
#include <list>

#include "miso.h"

#include "geometry.h"
#include "window.h"
#include "button.h"


namespace isolinear::ui::header {

  class basic : public Drawable {
    protected:
      Grid grid;
      Window& window;
      Compass alignment{CENTRE};
      std::string text{""};

    public:
      basic(Grid g, Window& w, Compass a)
        : basic(g, w, a, "")
      {}

      basic(Grid g, Window& w, Compass a, std::string t)
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

  class east_bar : public Drawable {
    protected:
      Grid grid;
      Window& window;
      std::string text{""};
      std::map<std::string, isolinear::ui::button> buttons;
      int button_width{2};

    public:
      east_bar(Grid g, Window& w, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      east_bar(Grid g, Window& w, Compass a, std::string t)
        : grid{g}, window{w}, text{t}
      {};

      east_bar(Grid g, Window& w, Compass a)
        : grid{g}, window{w}
      {};

      void Label(std::string newlabel) {
          text = newlabel;
      }
      virtual ColourScheme Colours() const {
        return Drawable::Colours();
      }

      virtual void Colours(ColourScheme cs) {
        for (auto& [label, button] : buttons) {
          button.Colours(cs);
        }
        Drawable::Colours(cs);
      }

      isolinear::ui::button& AddButton(std::string label) {
        buttons.try_emplace(
            label,
            window,
            ButtonRegion(buttons.size() + 1),
            label
          );
        return buttons.at(label);
      }

      Region2D ButtonRegion(int i) const  {
        i = (i-1) * button_width;
        return grid.CalculateGridRegion(
            2+i,   1,
            2+i+1, 2
          );
      }

      virtual void OnPointerEvent(PointerEvent event) override {
        for (auto& [label, button] : buttons) {
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
          for (auto const& [label, button] : buttons) {
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

  class pair_bar : public Drawable {
    protected:
      Grid grid;
      Window& window;
      std::string left{""};
      std::string right{""};

    public:
      pair_bar(Grid g, Window& w,
          std::string l, std::string r)
        : grid{g}, window{w}, left{l}, right{r}
      {};

      pair_bar(Grid g, Window& w, Compass a)
        : grid{g}, window{w}
      {};

      void Left(std::string newlabel) {
          left = newlabel;
      }

      void Right(std::string newlabel) {
          right = newlabel;
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
        Region2D left_cap = grid.CalculateGridRegion(
            1, 1,
            1, grid.MaxRows()
          );

        Region2D centre_bar = grid.CalculateGridRegion(
            2, 1,
            grid.MaxColumns() - 1, grid.MaxRows()
          );

        Region2D right_cap = grid.CalculateGridRegion(
            grid.MaxColumns(), 1,
            grid.MaxColumns(), grid.MaxRows()
          );

        std::string paddedleft = " " + left + " ";
        std::string paddedright = " " + right + " ";

        auto const& headerfont = window.HeaderFont();
        RenderedText  lefttext = headerfont.RenderText(
            Colours().active, paddedleft
          );
        RenderedText righttext = headerfont.RenderText(
            Colours().active, paddedright
          );

        Region2D lefttextregion = centre_bar.Align(
            Compass::WEST, lefttext.Size()
          );
        Region2D righttextregion = centre_bar.Align(
            Compass::EAST, righttext.Size()
          );

        Position2D leftlimit = lefttextregion.SouthEast();
        Position2D rightlimit = righttextregion.NorthWest();

        int left_text_end_col_index = grid.PositionColumnIndex(
            lefttextregion.East()
          );
        int right_text_end_col_index = grid.PositionColumnIndex(
            righttextregion.West()
          );

        Region2D drawcentrebar = grid.CalculateGridRegion(
            left_text_end_col_index + 1, 1,
            right_text_end_col_index - 1, grid.MaxRows()
          );

        Region2D left_text_end_cell =
          grid.CalculateGridRegion(
              left_text_end_col_index, 1,
              left_text_end_col_index, grid.MaxRows()
            );

        Region2D right_text_end_cell =
          grid.CalculateGridRegion(
              right_text_end_col_index, 1,
              right_text_end_col_index, grid.MaxRows()
            );

        Region2D left_text_filler{
            Position2D(leftlimit.x, left_text_end_cell.NorthWestY()),
            left_text_end_cell.SouthEast()
          };

        Region2D right_text_filler{
            right_text_end_cell.NorthWest(),
            Position2D(rightlimit.x, right_text_end_cell.SouthEastY())
          };

        if (right_text_filler.W() >= grid.Gutter().x) {
          right_text_filler.Fill(renderer, Colours().light);
        }

        if (left_text_filler.W() >= grid.Gutter().x) {
          left_text_filler.Fill(renderer, Colours().light);
        }

        drawcentrebar.Fill(renderer, Colours().frame);
        left_cap.Bullnose(renderer, Compass::WEST, Colours().light);
        right_cap.Bullnose(renderer, Compass::EAST, Colours().light);

        lefttext.Draw(renderer, Compass::WEST, centre_bar);
        righttext.Draw(renderer, Compass::EAST, centre_bar);
      }
  };

}
