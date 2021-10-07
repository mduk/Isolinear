#include "window.h"

void Window::Add(Drawable* drawable) {
  this->drawables.push_back(drawable);
}

void Window::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  Position mouse_position(event->x, event->y);
  printf("Window Click: %dx%d\n", mouse_position.x, mouse_position.y);
  for (auto const * d : this->drawables) {
    printf(".\n");
  }
}

void Window::Draw(SDL_Renderer* renderer) {
  for (auto const * drawable : this->drawables) {
    drawable->Draw(renderer);
  }
}

