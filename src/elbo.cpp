#include "elbo.h"

void Elbo::Draw(SDL_Renderer* renderer) {
  Region* top_bar = new Region(
    this->corner->x,
    this->bounds->position->y + this->gutter,
    this->bounds->size->x - this->corner->x - this->gutter,
    this->corner->y - this->gutter
  );

  Region* side_bar = new Region(
    this->bounds->position->x + this->gutter,
    this->bounds->position->y + this->gutter + outer_radius,
    this->corner->x - this->gutter,
    this->bounds->size->y - outer_radius - this->gutter - this->gutter
  );

  int rox = this->bounds->position->x + this->gutter + this->outer_radius,
      roy = this->bounds->position->y + this->gutter + this->outer_radius;

  filledCircleColor(
    renderer,
    rox, roy,
    this->outer_radius,
    this->colour
  );

  boxColor(renderer,
    this->bounds->position->x + this->gutter + this->outer_radius,
    this->bounds->position->y + this->gutter,
    this->corner->x + this->gutter,
    this->bounds->position->y + this->gutter + this->outer_radius,
    0xffffff00 // this->colour
  );

  boxColor(renderer,
    rox, this->bounds->position->x + this->gutter,
    rox + this->outer_radius, this->outer_radius + this->gutter,
    0xff000000
  );

  boxColor(renderer,
    rox, roy,
    rox + this->outer_radius, roy + this->outer_radius,
    0xff000000
  );

  side_bar->Draw(renderer, this->colour);
  top_bar->Draw(renderer, this->colour);

  int rix = this->corner->x + this->inner_radius,
      riy = this->corner->y + this->inner_radius;

  boxColor(renderer,
    this->corner->x,
    this->corner->y,
    this->corner->x + this->inner_radius,
    this->corner->y + this->inner_radius,
    this->colour
  );

  filledCircleColor(renderer,
    rix, riy,
    this->inner_radius,
    0xff000000
  );

  this->corner->Draw(renderer, 0xffffffff);

  delete top_bar, side_bar;
}

Region* Elbo::ContainerRegion() {
  return new Region(
    this->corner->x + this->gutter,
    this->corner->y + this->gutter,
    this->bounds->size->x - (this->corner->x + this->gutter),
    this->bounds->size->y - (this->corner->y + this->gutter)
  );
}
