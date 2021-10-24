#include "elbo.h"

void NorthWestElbo::Draw(SDL_Renderer* renderer) const {
  Region2D sweep = SweepRegion();
  Region2D vertical = VerticalRegion();

  Region2D reach = ReachRegion();
  Region2D header = HeaderRegion();

  Region2D outer_radius{
      sweep.Origin(),
      Size2D{sweep.W()/3}
    };

  Region2D inner_corner = sweep.AlignSouthEast(
      Size2D{
          sweep.FarX() - vertical.FarX(),
          header.H() + gutter.y
        }
    );

  Region2D inner_radius = inner_corner.AlignNorthWest(
      Size2D{sweep.W()/4}
    );


  boxColor(renderer,
      sweep.NearX(), sweep.NearY(),
      sweep.FarX(), sweep.FarY(),
      colours.base
    );

  boxColor(renderer,
      outer_radius.NearX(), outer_radius.NearY(),
      outer_radius.FarX(), outer_radius.FarY(),
      0xff000000
    );

  filledPieColor(renderer,
      outer_radius.FarX(), outer_radius.FarY(),
      outer_radius.H(),
      180, 270,
      colours.base
    );

  boxColor(renderer,
      inner_corner.NearX(), inner_corner.NearY(),
      inner_corner.FarX(), inner_corner.FarY(),
      0xff000000
    );

  boxColor(renderer,
      inner_radius.NearX(), inner_radius.NearY(),
      inner_radius.FarX(), inner_radius.FarY(),
      colours.base
    );

  filledPieColor(renderer,
      inner_radius.FarX(), inner_radius.FarY(),
      inner_radius.H(),
      180, 270,
      0xff000000
    );

  boxColor(renderer,
      reach.NearX(), reach.NearY(),
      reach.FarX(), reach.FarY(),
      colours.base
    );

  boxColor(renderer,
      vertical.NearX(), vertical.NearY(),
      vertical.FarX(), vertical.FarY(),
      colours.base
    );

  window.HeaderFont().RenderTextWest(
      renderer,
      header,
      header_string
    );
}

void SouthWestElbo::Draw(SDL_Renderer* renderer) const {
  Region2D sweep = SweepRegion();
  Region2D vertical = VerticalRegion();

  Region2D reach = ReachRegion();
  Region2D header = HeaderRegion();

  boxColor(renderer,
      sweep.NearX(), sweep.NearY(),
      sweep.FarX(), sweep.FarY(),
      colours.base
    );

  Region2D outer_radius = sweep.AlignSouthWest(Size2D{sweep.W()/3});

  boxColor(renderer,
      outer_radius.NearX(), outer_radius.NearY(),
      outer_radius.FarX(), outer_radius.FarY(),
      0xff000000
    );

  filledPieColor(renderer,
      outer_radius.NorthEastX(),
      outer_radius.NorthEastY(),
      outer_radius.H(),
      90, 180,
      colours.base
    );

  boxColor(renderer,
      reach.NearX(), reach.NearY(),
      reach.FarX(), reach.FarY(),
      colours.base
    );

  boxColor(renderer,
      vertical.NearX(), vertical.NearY(),
      vertical.FarX(), vertical.FarY(),
      colours.base
    );

  window.HeaderFont().RenderTextWest(
      renderer,
      header,
      header_string
    );

}
