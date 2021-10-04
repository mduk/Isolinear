#include "window.h"

Window::Window()
  : drawables{}
{ }

void Window::Add(Drawable* drawable) {
  this->drawables.push_back(drawable);
}

void Window::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  Position* mouse_position = new Position(event->x, event->y);
  for (auto* drawable : this->drawables) {
    Region* bounds = drawable->Bounds();
    if (bounds) {
      printf("valid bounds\n");
    }
    else {
      printf("no bounds\n");
    }
  }
  delete mouse_position;
}

void Window::Draw(
    SDL_Renderer* renderer
) {
  for (auto* drawable : this->drawables) {
    drawable->Draw(renderer);
  }
}

