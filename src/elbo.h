#pragma once

#include <exception>
#include <list>
#include <stdexcept>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "grid.h"
#include "button.h"
#include "colours.h"
#include "geometry.h"
#include "window.h"
#include "shapes.h"


using namespace std;


class Elbo : public Drawable {

  protected:
    Window& window;
    Grid& grid;

    int reach_weight{30};
    Vector2D sweep_cells{2,2};
    Vector2D gutter{10,10};
    ColourScheme colours{ red_alert_colours };
    std::string header_string{""};
    Compass header_alignment{CENTRE};
    std::list<Button> buttons{};

  public:
    Elbo(
        Window& w,
        Grid& g,
        std::string h,
        Compass ha
      ) :
        window{w},
        grid{g},
        header_string{h},
        header_alignment{ha}
    {};

    virtual Region2D ContainerRegion() const = 0;
    virtual Region2D VerticalRegion() const = 0;
    virtual Region2D SweepRegion() const = 0;
    virtual Region2D SweepOuterRadiusRegion() const = 0;
    virtual Region2D SweepInnerCornerRegion() const = 0;
    virtual Region2D SweepInnerRadiusRegion() const = 0;
    virtual Region2D HorizontalRegion() const = 0;
    virtual Region2D ButtonRegion(int) const = 0;

    virtual Region2D ReachRegion() const = 0;
    virtual Region2D HeaderRegion() const = 0;

    void Colours(ColourScheme cs) {
      colours = cs;
    }

    virtual int SweepOuterRadius() const {
      return min(
          SweepRegion().W() / 2,
          VerticalRegion().W()
        );
    }

    virtual int SweepInnerRadius() const {
      return SweepRegion().H() / 2;
    }

    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      Position2D cursor{event};

      auto const container_region = ContainerRegion();
      if (container_region.Encloses(cursor)) {
        printf("Container: ");
        container_region.Print();
        return;
      }

      auto const vertical_region = VerticalRegion();
      if (vertical_region.Encloses(cursor)) {
        printf("Vertical: ");
        vertical_region.Print();
        return;
      }

      for (auto& button : buttons) {
        if (button.Encloses(cursor)) {
          button.OnMouseButtonDown(event);
          return;
        }
      }

      auto const sweep_region = SweepRegion();
      if (sweep_region.Encloses(cursor)) {
        printf("Sweep: ");
        sweep_region.Print();
        return;
      }

      auto const horizontal_region = HorizontalRegion();
      if (horizontal_region.Encloses(cursor)) {
        printf("Horizontal: ");
        horizontal_region.Print();

        auto const reach_region = ReachRegion();
        if (reach_region.Encloses(cursor)) {
          printf("Reach: ");
          reach_region.Print();
          return;
        }

        auto const header_region = HeaderRegion();
        if (header_region.Encloses(cursor)) {
          printf("Header: ");
          header_region.Print();
          return;
        }

        return;
      }

    };

    SDL_Rect SdlRect() const {
      return grid.bounds.SdlRect();
    }

    void AddButton(std::string label) {
      buttons.emplace_back(
          window,
          ButtonRegion(buttons.size() + 1),
          colours,
          label
        );
    }

    virtual void Draw(SDL_Renderer* renderer) const {
      DrawSweep(renderer);
      //SweepRegion().Draw(renderer);
      DrawReach(renderer);
      DrawVertical(renderer);
      DrawHeader(renderer);
      for (auto const& button : buttons) {
        button.Draw(renderer);
      }
      if (false) {
        SweepInnerCornerRegion().Draw(renderer);
        SweepInnerRadiusRegion().Draw(renderer);
        SweepOuterRadiusRegion().Draw(renderer);
      }
    }

    virtual void DrawSweep(SDL_Renderer* renderer) const {
      Region2D sweep = SweepRegion();
      Region2D outer_radius = SweepOuterRadiusRegion();
      Region2D inner_corner = SweepInnerCornerRegion();
      Region2D inner_radius = SweepInnerRadiusRegion();

      boxColor(renderer,
          sweep.NearX(), sweep.NearY(),
          sweep.FarX(), sweep.FarY(),
          colours.frame
        );

      boxColor(renderer,
          outer_radius.NearX(), outer_radius.NearY(),
          outer_radius.FarX(), outer_radius.FarY(),
          colours.background
        );

      boxColor(renderer,
          inner_corner.NearX(), inner_corner.NearY(),
          inner_corner.FarX(), inner_corner.FarY(),
          colours.background
        );

      boxColor(renderer,
          inner_radius.NearX(), inner_radius.NearY(),
          inner_radius.FarX(), inner_radius.FarY(),
          colours.frame
        );

    };

    virtual void DrawReach(SDL_Renderer* renderer) const {
      Region2D reach = ReachRegion();
      boxColor(renderer,
          reach.NearX(), reach.NearY(),
          reach.FarX(), reach.FarY(),
          colours.frame
        );
    }

    virtual void DrawHeader(SDL_Renderer* renderer) const {
      window.HeaderFont().RenderText(
          renderer,
          HeaderRegion(),
          header_alignment,
          header_string
        );
    }

    virtual void DrawVertical(SDL_Renderer* renderer) const {
      Region2D vertical = VerticalRegion();
      boxColor(renderer,
          vertical.NearX(), vertical.NearY(),
          vertical.FarX(), vertical.FarY(),
          colours.frame
        );
    }
};


// // // // // // // // // // // // // // // // // // // //


class NorthWestElbo : public Elbo {

  public:
    NorthWestElbo(Window& w, Grid& g, std::string h)
      : Elbo(w, g, h, Compass{NORTHWEST})
    {}

  protected:
    Region2D SweepRegion() const override {
      return grid.CalculateGridRegion(
        1, 1,
        sweep_cells.x, sweep_cells.y
      );
    }

    Region2D SweepOuterRadiusRegion() const override {
      Region2D sweep = SweepRegion();

      return Region2D{
          sweep.Origin(),
          Size2D{SweepOuterRadius()}
        };
    }

    Region2D SweepInnerCornerRegion() const override {
      Region2D sweep = SweepRegion();

      return sweep.Align(
          Compass{SOUTHEAST},
          Size2D{
              sweep.FarX() - VerticalRegion().FarX(),
              HeaderRegion().H() + gutter.y
            }
        );
    }

    Region2D SweepInnerRadiusRegion() const override {
      return SweepInnerCornerRegion().Align(
          Compass{NORTHWEST},
          Size2D{SweepInnerRadius()}
        );
    }

    Region2D HorizontalRegion() const override {
      return grid.CalculateGridRegion(
        sweep_cells.x + 1, 1,
        grid.MaxColumns(), 2
      );
    }

    Region2D ReachRegion() const override {
      Region2D horizontal = HorizontalRegion();

      return Region2D{
          horizontal.Origin(),
          Size2D{
            horizontal.W(),
            reach_weight
          }
        };
    }

    Region2D HeaderRegion() const override {
      Region2D horizontal = HorizontalRegion();

      return Region2D{
          Position2D{
            horizontal.Origin().x,
            horizontal.Origin().y
              + reach_weight
              + gutter.y
          },
          Size2D{
            horizontal.Size().x,
            horizontal.Size().y
              - reach_weight
              - gutter.y
          }
        };
    }

    Region2D VerticalRegion() const override {
      return grid.CalculateGridRegion(
          1, sweep_cells.y + 1 + buttons.size(),
          sweep_cells.x - 1, grid.MaxRows()
        );
    }

    Region2D ContainerRegion() const override {
      return grid.CalculateGridRegion(
          sweep_cells.x, sweep_cells.y + 1,
          grid.MaxColumns(), grid.MaxRows()
        );
    }

    Region2D ButtonRegion(int i) const override {
      return grid.CalculateGridRegion(
          1, sweep_cells.y +  i,
          sweep_cells.x - 1, sweep_cells.y  + i
        );
    }

    void DrawSweep(SDL_Renderer* renderer) const {
      Region2D outer_radius = SweepOuterRadiusRegion();
      Region2D inner_radius = SweepInnerRadiusRegion();

      Elbo::DrawSweep(renderer);

      filledPieColor(renderer,
          outer_radius.FarX(), outer_radius.FarY(),
          outer_radius.H(),
          180, 270,
          colours.frame
        );

      filledPieColor(renderer,
          inner_radius.FarX(), inner_radius.FarY(),
          inner_radius.H(),
          180, 270,
          colours.background
        );
    }

};



class SouthWestElbo : public Elbo {

  public:
    SouthWestElbo(Window& w, Grid& g, std::string h)
      : Elbo(w, g, h, Compass{SOUTHWEST})
    {}

  protected:
    Region2D SweepRegion() const override {
      return grid.CalculateGridRegion(
        1, grid.MaxRows() - sweep_cells.y + 1,
        sweep_cells.x, grid.MaxRows()
      );
    }

    Region2D SweepOuterRadiusRegion() const override {
      Region2D sweep = SweepRegion();

      return sweep.Align(
          Compass{SOUTHWEST},
          Size2D{SweepOuterRadius()}
        );
    }

    Region2D SweepInnerCornerRegion() const override {
      Region2D sweep = SweepRegion();
      Region2D vertical = VerticalRegion();
      Region2D header = HeaderRegion();

      return sweep.Align(
          Compass{NORTHEAST},
          Size2D{
              sweep.FarX() - vertical.FarX(),
              header.H() + gutter.y
            }
        );
    }

    Region2D SweepInnerRadiusRegion() const override {
      Region2D sweep = SweepRegion();

      return SweepInnerCornerRegion().Align(
          Compass{SOUTHWEST},
          Size2D{SweepInnerRadius()}
        );
    }

    Region2D HorizontalRegion() const override {
      return grid.CalculateGridRegion(
        sweep_cells.x + 1, grid.MaxRows() - sweep_cells.y + 1,
        grid.MaxColumns(), grid.MaxRows()
      );
    }

    Region2D ReachRegion() const override {
      Region2D horizontal = HorizontalRegion();

      return Region2D{
          Position2D{
            horizontal.X(),
            horizontal.Y() + horizontal.H() - reach_weight
          },
          Size2D{
              horizontal.W(),
              reach_weight
            }
        };
    }

    Region2D HeaderRegion() const override {
      Region2D horizontal = HorizontalRegion();

      return Region2D{
          Position2D{
            horizontal.X(),
            horizontal.Y()
          },
          Size2D{
              horizontal.W(),
              horizontal.H() - reach_weight - gutter.y
            }
        };
    }

    Region2D VerticalRegion() const override {
      return grid.CalculateGridRegion(
          1,1,
          sweep_cells.x -1, grid.MaxRows() - sweep_cells.y - buttons.size()
        );
    }

    Region2D ContainerRegion() const override {
      return Region2D{ };
    }

    Region2D ButtonRegion(int i) const override {
      return grid.CalculateGridRegion(
          sweep_cells.x - 1, grid.MaxRows() - sweep_cells.y - buttons.size(),
          sweep_cells.x - 1, grid.MaxRows() - sweep_cells.y - buttons.size()
        );
    };

    void DrawSweep(SDL_Renderer* renderer) const override {
      Region2D outer_radius = SweepOuterRadiusRegion();
      Region2D inner_radius = SweepInnerRadiusRegion();

      Elbo::DrawSweep(renderer);

      filledPieColor(renderer,
          outer_radius.NorthEastX(),
          outer_radius.NorthEastY(),
          outer_radius.H(),
          90, 180,
          colours.frame
        );


      filledPieColor(renderer,
          inner_radius.NorthEastX(),
          inner_radius.NorthEastY(),
          inner_radius.H(),
          90, 180,
          colours.background
        );
    }
};
