#pragma once

#include <exception>
#include <list>
#include <stdexcept>

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

    int reach_weight{10};
    Vector2D sweep_cells{2,2};
    Vector2D gutter{10,10};
    ColourScheme colours{
        0xff664466, 0xffcc9999,
        0xffff9999, 0xff6666cc
      };
    std::string header_string{""};
    std::list<Button> buttons{};

  public:
    Elbo(
        Window& w,
        std::string h
      ) :
        Elbo{ w, w.grid, h }
    {};

    Elbo(
        Window& w,
        Grid& g,
        std::string h
      ) :
        window{w},
        grid{g},
        header_string{h}
    {};



    virtual Region2D ContainerRegion() const = 0;
    virtual Region2D VerticalRegion() const = 0;
    virtual Region2D SweepRegion() const = 0;
    virtual Region2D HorizontalRegion() const = 0;

    virtual Region2D ReachRegion() const = 0;
    virtual Region2D HeaderRegion() const = 0;

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

    void Draw(SDL_Renderer* renderer) const {
      ContainerRegion().Draw(renderer);
      VerticalRegion().Draw(renderer);
      //HorizontalRegion().Draw(renderer);
      SweepRegion().Draw(renderer);
      ReachRegion().Draw(renderer);
      HeaderRegion().Draw(renderer);
    }

    SDL_Rect SdlRect() const {
      return grid.bounds.SdlRect();
    }

    void AddButton(std::string label) {
      buttons.emplace_back(
          window,
          Region2D{},
          colours,
          label
        );
    }

};


class NorthWestElbo : public Elbo {

  public:
    NorthWestElbo(Window& w, Grid& g, std::string h)
      : Elbo(w, g, h)
    {}

    void Draw(SDL_Renderer*) const;

  protected:
    Region2D SweepRegion() const override {
      return grid.CalculateGridRegion(
        1, 1,
        sweep_cells.x, sweep_cells.y
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
        1, sweep_cells.y + 1,
        sweep_cells.x - 1, grid.MaxRows()
      );
    }

    Region2D ContainerRegion() const override {
      return grid.CalculateGridRegion(
        sweep_cells.x, sweep_cells.y + 1,
        grid.MaxColumns(), grid.MaxRows()
      );
    }
};



class SouthWestElbo : public Elbo {

  public:
    SouthWestElbo(Window& w, Grid& g, std::string h)
      : Elbo(w, g, h)
    {}

    void Draw(SDL_Renderer*) const;

  protected:
    Region2D SweepRegion() const override {
      return grid.CalculateGridRegion(
        1, grid.MaxRows() - sweep_cells.y + 1,
        sweep_cells.x, grid.MaxRows()
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
          sweep_cells.x -1, grid.MaxRows() - sweep_cells.y
        );
    }

    Region2D ContainerRegion() const override {
      return Region2D{ };
    }



};
