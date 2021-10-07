#include "window.h"

Window::Window(int w, int h)
  : width_px{w}, height_px{h}
{
  this->sdl_window = SDL_CreateWindow(
    "Isolinear",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    this->width_px, this->height_px,
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
    }
  }
}


void Button::Draw(SDL_Renderer* renderer) const {
  boxColor(renderer,
    this->bounds.NearX(), this->bounds.NearY(),
    this->bounds.FarX(), this->bounds.FarY(),
    0xffffffff
  );
}

void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Button clicked!\n");
}

Region Button::Bounds() {
  return this->bounds;
}
