#include "window.h"

Window::Window()
  : drawables{}
{ }

void Window::Add(Drawable* drawable) {
  this->drawables.push_back(drawable);
}

void Window::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  Position* mouse_position = new Position(event->x, event->y);
  for (auto const* drawable : this->drawables) {
    //
  }
  delete mouse_position;
}

void Window::Draw(
    SDL_Renderer* renderer
) {
  for (auto const* drawable : this->drawables) {
    drawable->Draw(renderer);
  }
}

