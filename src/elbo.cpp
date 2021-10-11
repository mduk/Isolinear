#include "elbo.h"


void Button::Draw(SDL_Renderer* renderer) const {
  Colour drawcolour = (this->active == true)
                    ? this->colours.active
                    : this->colours.base;
  boxColor(renderer,
    this->bounds.NearX(), this->bounds.NearY(),
    this->bounds.FarX(), this->bounds.FarY(),
    drawcolour
  );
}

void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Button::OnMouseButtonDown\n");
  this->active = !this->active;
}

Region Button::Bounds() {
  return this->bounds;
}




Region Elbo::SweepRegion() const {
  return Region{
      this->bounds.position,
      Size{ this->corner.size.x + inner_radius,
            this->corner.size.y + inner_radius }
    };
}

Region Elbo::HorizontalRegion() const {
  Region sweep = this->SweepRegion();
  return Region{
      Position{ sweep.FarX(), sweep.NearY() },
      Size{ bounds.size.x - corner.size.x - inner_radius,
            corner.size.y
      }
    };
}

Region Elbo::VerticalRegion() const {
  Region sweep = this->SweepRegion();
  return Region{
      Position{ sweep.NearX(),
                sweep.FarY() + gutter.y
      },
      Size{ corner.size.x,
            bounds.size.y - sweep.size.y - gutter.y
      }
    };
}

Region Elbo::InnerRadiusRegion() const {
  return Region{
      Position{ corner.Far() },
      Size{ inner_radius, inner_radius }
    };
}

Region Elbo::OuterRadiusRegion() const {
  return Region{
      bounds.position,
      Size{ outer_radius, outer_radius }
    };
}

Region Elbo::HeaderRegion() const {
  Region sweep = SweepRegion();
  return Region{
      Position{
          sweep.FarX(),
          sweep.FarY() - inner_radius
      },
      Size{
          bounds.size.x - sweep.size.x,
          inner_radius
      }
    };
}

Region Elbo::ContainerRegion() const {
  return Region{
    Position{corner.FarX(), corner.FarY() + inner_radius},
    Size{bounds.size.x - corner.size.x,
         bounds.size.y - corner.size.y - inner_radius}
    };
}


void Elbo::AddButton() {
  Grid g{
    this->VerticalRegion(),
    NumCols{1},
    Margin{0, 0},
    Gutter{0, 10}
  };
  buttons.emplace_back(
      g.SingleCellRegion(1, buttons.size() + 1),
      colours
  );
}


void Elbo::OnMouseButtonDown(SDL_MouseButtonEvent& e) {
  printf("Elbo::OnMouseButtonDown\n");
  Position cursor{e};

  if (SweepRegion().Encloses(cursor)) {
    printf("      Sweep click\n");
    return;
  }

  if (HorizontalRegion().Encloses(cursor)) {
    printf("      Horizontal click\n");
    return;
  }

  if (VerticalRegion().Encloses(cursor)) {
    printf("      Vertical click\n");
    for (auto button : buttons) {
      if (button.Bounds().Encloses(cursor)) {
        button.OnMouseButtonDown(e);
        return;
      }
    }
    // Tail region
    return;
  }

  if (ContainerRegion().Encloses(cursor)) {
    printf("      Container click\n");
    return;
  }
}

void Elbo::Draw(SDL_Renderer* renderer) const {
  Region sweep = this->SweepRegion();
  boxColor(renderer,
    sweep.NearX(), sweep.NearY(),
    sweep.FarX(),  sweep.FarY(),
    this->colours.base
  );

  Region hbar = this->HorizontalRegion();
  boxColor(renderer,
    hbar.NearX(), hbar.NearY(),
    hbar.FarX(),  hbar.FarY(),
    this->colours.base
  );

  Region vbar = this->VerticalRegion();
  rectangleColor(renderer,
    vbar.NearX(), vbar.NearY(),
    vbar.FarX(),  vbar.FarY(),
    this->colours.base
  );

  for (auto b : this->buttons) {
    b.Draw(renderer);
  }

  Region oradius = this->OuterRadiusRegion();
  boxColor(renderer,
    oradius.NearX(), oradius.NearY(),
    oradius.FarX(),  oradius.FarY(),
    0xff000000
  );
  filledPieColor(renderer,
    oradius.FarX(), oradius.FarY(),
    outer_radius,
    180, 270,
    this->colours.base
  );

  Region iradius = this->InnerRadiusRegion();
  filledPieColor(renderer,
    iradius.FarX(), iradius.FarY(),
    inner_radius,
    180, 270,
    0xff000000
  );

  Region header = HeaderRegion();

  Region container = this->ContainerRegion();
  rectangleColor(renderer,
    container.NearX(), container.NearY(),
    container.FarX(),  container.FarY(),
    0xff000000
  );

  if (false) { // debug
    rectangleColor(renderer,
        bounds.NearX(), bounds.NearY(),
        bounds.FarX() , bounds.FarY(),
        0xffffffff
    );

    rectangleColor(renderer,
        iradius.NearX(), iradius.NearY(),
        iradius.FarX() , iradius.FarY(),
        0xffffffff
    );

    rectangleColor(renderer,
        oradius.NearX(), oradius.NearY(),
        oradius.FarX() , oradius.FarY(),
        0xffffffff
    );

    rectangleColor(renderer,
        this->corner.NearX(), this->corner.NearY(),
        this->corner.FarX() , this->corner.FarY(),
        0xffffffff
    );

    rectangleColor(renderer,
      header.NearX(), header.NearY(),
      header.FarX(),  header.FarY(),
      0xffffffff
    );

    rectangleColor(renderer,
      container.NearX(), container.NearY(),
      container.FarX(),  container.FarY(),
      0xffffffff
    );

  }
}

