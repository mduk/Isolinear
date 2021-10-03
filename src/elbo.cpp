#include "elbo.h"

void Elbo::OnMouseButtonDown(SDL_MouseButtonEvent* e) {}

void Elbo::Draw(SDL_Renderer* renderer) {
  int outer_radius_origin_x
    = bounds->position->x
    + outer_radius;

  int outer_radius_origin_y
    = bounds->position->y
    + outer_radius;

  Region* sweep_fill = new Region(
    bounds->position->x + outer_radius,
    bounds->position->y,
    (bounds->position->x + corner->x) - (bounds->position->x + outer_radius),
    outer_radius
  );

  Region* sweep_south = new Region(
    bounds->position->x,
    bounds->position->y + outer_radius,
    corner->x,
    sweep_base_height
  );

  Region* sweep_shelf = new Region(
      bounds->position->x + corner->x, bounds->position->y,
      inner_radius, inner_radius
  );

  // sweep_arc
  filledPieColor(renderer,
    outer_radius_origin_x,
    outer_radius_origin_y,

    outer_radius,
    180, 270,

    colour
  );

  sweep_fill->Draw(renderer, colour);
  sweep_shelf->Draw(renderer, colour);
  sweep_south->Draw(renderer, colour);

  Region* top_bar = new Region(
    bounds->position->x + corner->x + inner_radius + gutter,
    bounds->position->y,
    bounds->size->x - corner->x - inner_radius - gutter,
    corner->y
  );

  Region* side_bar = new Region(
    bounds->position->x,
    bounds->position->y + outer_radius + sweep_base_height + gutter,
    corner->x,
    bounds->size->y - outer_radius - gutter - gutter
  );

  side_bar->Draw(renderer, colour);
  top_bar->Draw(renderer, colour);


  int rix = bounds->position->x + corner->x + inner_radius,
      riy = bounds->position->y + corner->y + inner_radius;

  boxColor(renderer,
    bounds->position->x + corner->x,
    bounds->position->y + corner->y,
    bounds->position->x + corner->x + inner_radius,
    bounds->position->y + corner->y + inner_radius,
    colour
  );

  filledCircleColor(renderer,
    rix+1, riy+1,
    inner_radius,
    0xff000000
  );

  delete top_bar, side_bar;
}

Region* Elbo::ContainerRegion() {
  return new Region(
    bounds->position->x + corner->x + gutter,
    bounds->position->y + corner->y + inner_radius + sweep_base_height,
    bounds->size->x - (corner->x + gutter),
    bounds->size->y - (corner->y + gutter) - inner_radius
  );
}
