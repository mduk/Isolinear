#include "window.h"

Window::Window(int w, int h)
  : size{w, h}
{
  this->sdl_window = SDL_CreateWindow(
    "Isolinear",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    this->size.x, this->size.y,
    SDL_WINDOW_OPENGL //| SDL_WINDOW_FULLSCREEN_DESKTOP
  );

  if (!this->sdl_window) {
    throw std::runtime_error(
      "Failed to create SDL window"
    );
  }

  this->sdl_renderer = SDL_CreateRenderer(
    this->sdl_window, -1, SDL_RENDERER_SOFTWARE
  );

  if (!this->sdl_renderer) {
    throw std::runtime_error(
      "Failed to create SDL renderer"
    );
  }

  SDL_SetRenderDrawBlendMode(
    this->sdl_renderer, SDL_BLENDMODE_BLEND
  );
  SDL_SetRenderDrawColor(this->sdl_renderer, 0, 0, 0, 255);
  SDL_RenderPresent(this->sdl_renderer);

  SDL_GetWindowSize(
    this->sdl_window,
    &this->size.x,
    &this->size.y
  );
}

void Window::Add(Drawable* drawable) {
  this->drawables.push_back(drawable);
}

void Window::Draw() {
  for (auto* drawable : this->drawables) {
    drawable->Draw(this->sdl_renderer);
  }
  SDL_RenderPresent(this->sdl_renderer);
}

void Window::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  Position cursor{event.x, event.y};

  for (auto* drawable : this->drawables) {
    if (drawable->Bounds().Encloses(cursor)) {
      drawable->OnMouseButtonDown(event);
      return;
    }
  }
}

void Button::Draw(SDL_Renderer* renderer) const {
  Colour drawcolour = (this->active == true)
                    ? this->colours.active
                    : this->colours.base;
  drawcolour = 0xffffffff;

  printf("Button: %d,%d %d,%d\n", this->bounds.NearX(), this->bounds.NearY(), this->bounds.size.x, this->bounds.size.y);
  boxColor(renderer,
    this->bounds.NearX(), this->bounds.NearY(),
    this->bounds.FarX(), this->bounds.FarY(),
    drawcolour
  );
}

void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  this->active = !this->active;
}

Region Button::Bounds() {
  return this->bounds;
}




Region Elbo::SweepRegion() const {
  return Region{
      this->bounds.position,
      Size{this->corner.size.x + inner_radius,
           this->corner.size.y + inner_radius}
    };
}

Region Elbo::HorizontalRegion() const {
  Region sweep = this->SweepRegion();
  return Region{
      Position{sweep.FarX() + gutter, sweep.NearY()},
      Size{bounds.size.x - corner.size.x - gutter, corner.size.y}
    };
}

Region Elbo::VerticalRegion() const {
  Region sweep = this->SweepRegion();
  return Region{
      Position{sweep.NearX(), sweep.FarY() + gutter},
      Size{corner.size.x, bounds.size.y - sweep.size.y - gutter}
    };
}

Region Elbo::InnerRadiusRegion() const {
  return Region{
      Position{corner.Far()},
      Size{inner_radius, inner_radius}
    };
}

Region Elbo::OuterRadiusRegion() const {
  return Region{
      bounds.position,
      Size{outer_radius, outer_radius}
    };
}

Region Elbo::ContainerRegion() const {
  return Region{
    Position{corner.FarX(), corner.FarY() + inner_radius},
    Size{bounds.size.x - corner.size.x,
         bounds.size.y - corner.size.y - inner_radius}
    };
}


void Elbo::AddButton(SDL_Renderer* renderer) {
  Grid g(this->ContainerRegion(), 1);
  int n = buttons.size();
  Region button_region = g.SingleCellRegion(1, n + 1);
  boxColor(renderer,
    button_region.NearX(), button_region.NearY(),
    button_region.FarX(),  button_region.FarY(),
    0xff0000ff
  );
  buttons.emplace_back(button_region, colours);
}


void Elbo::OnMouseButtonDown(SDL_MouseButtonEvent&) {
  printf("my elbo feel funny\n");
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

  int i = 0;
  for (auto b : this->buttons) {
    i++;
    printf("Drawing button %d\n", i);
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

  Region container = this->ContainerRegion();
  boxColor(renderer,
    container.NearX(), container.NearY(),
    container.FarX(),  container.FarY(),
    0xff000000
  );

  if (false) { // debug
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
      container.NearX(), container.NearY(),
      container.FarX(),  container.FarY(),
      0xffffffff
    );

  }
}

