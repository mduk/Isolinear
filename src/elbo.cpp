#include "elbo.h"


Region2D Elbo::SweepRegion() const {
  return Region2D{
      bounds.Origin(),
      Size2D{ corner.W() + inner_radius,
            corner.H() + inner_radius }
    };
}

Region2D Elbo::HorizontalRegion() const {
  Region2D sweep = SweepRegion();
  return Region2D{
      Position2D{ sweep.FarX(), sweep.NearY() },
      Size2D{ bounds.W() - corner.W() - inner_radius,
            corner.H()
      }
    };
}

Region2D Elbo::VerticalRegion() const {
  Region2D sweep = SweepRegion();
  return Region2D{
      Position2D{ sweep.NearX(),
                sweep.FarY() + gutter.y
      },
      Size2D{ corner.W(),
            bounds.H() - sweep.H() - gutter.y
      }
    };
}

Region2D Elbo::InnerRadiusRegion() const {
  return Region2D{
      Position2D{ corner.FarX(), corner.FarY() + 1 },
      Size2D{ inner_radius, inner_radius }
    };
}

Region2D Elbo::OuterRadiusRegion() const {
  return Region2D{
      bounds.Origin(),
      Size2D{ outer_radius, outer_radius }
    };
}

Region2D Elbo::HeaderRegion() const {
  Region2D sweep = SweepRegion();
  return Region2D{
      Position2D{
          sweep.FarX(),
          sweep.FarY() - inner_radius
      },
      Size2D{
          bounds.W() - sweep.W(),
          inner_radius
      }
    };
}

Region2D Elbo::ContainerRegion() const {
  return Region2D{
    Position2D{corner.FarX(), corner.FarY() + inner_radius},
    Size2D{bounds.W() - corner.W(),
         bounds.H() - corner.H() - inner_radius}
    };
}


void Elbo::AddButton() {
  std::string tmp_s;
  static const char chars[] = "0123456789";

  int len = 3;
  tmp_s.reserve(len);
  for (int i = 0; i < len; ++i) {
      tmp_s += chars[rand() % (sizeof(chars) - 1)];
  }

  buttons.emplace_back(
      window,
      button_grid.SingleCellRegion(1, buttons.size() + 1),
      colours,
      tmp_s
    );
}


void Elbo::OnMouseButtonDown(SDL_MouseButtonEvent& e) {
  printf("Elbo::OnMouseButtonDown\n");
  Position2D cursor{e};

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
    for (auto& button : buttons) {
      if (button.Encloses(cursor)) {
        button.OnMouseButtonDown(e);
        return;
      }
    }
    // Tail region
    return;
  }

  if (ContainerRegion().Encloses(cursor)) {
    printf("      Container click\n");
    if (container) {
      container->OnMouseButtonDown(e);
    }
    return;
  }
}

void Elbo::Draw(SDL_Renderer* renderer) const {
  Region2D sweep = SweepRegion();
  Region2D hbar = HorizontalRegion();
  Region2D vbar = VerticalRegion();
  Region2D oradius = OuterRadiusRegion();
  Region2D iradius = InnerRadiusRegion();
  Region2D header_region = HeaderRegion();
  Region2D container = ContainerRegion();

  if (wireframe) {
    sweep.Stroke(renderer, 0xffffffff);
    hbar.Stroke(renderer, 0xffffffff);
    vbar.Stroke(renderer, 0xffffffff);
    oradius.Stroke(renderer, 0xffffffff);
    iradius.Stroke(renderer, 0xffffffff);
    header_region.Stroke(renderer, 0xffffffff);
    container.Stroke(renderer, 0xffffffff);
    return;
  }

  if (basic) {
    sweep.Fill(renderer, 0x88ffffff);
    hbar.Fill(renderer, 0x88ffffff);
    iradius.Fill(renderer, 0x88000000);
    vbar.Fill(renderer, 0x88ffffff);
    return;
  }

  boxColor(renderer,
    sweep.NearX(), sweep.NearY(),
    sweep.FarX(),  sweep.FarY(),
    colours.base
  );

  boxColor(renderer,
    hbar.NearX(), hbar.NearY(),
    hbar.FarX(),  hbar.FarY(),
    colours.base
  );

  rectangleColor(renderer,
    vbar.NearX(), vbar.NearY(),
    vbar.FarX(),  vbar.FarY(),
    colours.base
  );

  for (auto const &b : buttons) {
    b.Draw(renderer);
  }

  boxColor(renderer,
    oradius.NearX(), oradius.NearY(),
    oradius.FarX(),  oradius.FarY(),
    0xff000000
  );
  filledPieColor(renderer,
    oradius.FarX(), oradius.FarY(),
    outer_radius,
    180, 270,
    colours.base
  );

  filledPieColor(renderer,
    iradius.FarX(), iradius.FarY(),
    inner_radius,
    180, 270,
    0xff000000
  );

  window.HeaderFont().RenderTextWest(
    renderer, header_region, header
  );

}

