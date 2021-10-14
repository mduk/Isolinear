#include "elbo.h"


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
    RowHeight{100},
    NumCols{1},
    Margin{0, 0},
    Gutter{0, 10}
  };

  std::string tmp_s;
  static const char chars[] = "0123456789";

  int len = 3;
  tmp_s.reserve(len);
  for (int i = 0; i < len; ++i) {
      tmp_s += chars[rand() % (sizeof(chars) - 1)];
  }

  buttons.emplace_back(
      g.SingleCellRegion(1, buttons.size() + 1),
      colours,
      tmp_s
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

  if (HeaderRegion().Encloses(cursor)) {
    printf("      Header click\n");
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

  Region header_region = HeaderRegion();
  window.HeaderFont().RenderTextWest(
    renderer, header_region, this->header
  );

  Region container = this->ContainerRegion();
  boxColor(renderer,
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
      header_region.NearX(), header_region.NearY(),
      header_region.FarX(),  header_region.FarY(),
      0xffffffff
    );

    rectangleColor(renderer,
      container.NearX(), container.NearY(),
      container.FarX(),  container.FarY(),
      0xffffffff
    );

  }
}

