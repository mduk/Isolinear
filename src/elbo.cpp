#include "elbo.h"

void Elbo::Draw(SDL_Renderer* renderer) {
  int guttered_position_x
    = bounds->position->x
    + gutter;

  int guttered_position_y
    = bounds->position->y
    + gutter;

  int outer_radius_origin_x
    = guttered_position_x
    + outer_radius;

  int outer_radius_origin_y
    = guttered_position_y
    + outer_radius;

  Region* sweep_fill = new Region(
    guttered_position_x + outer_radius,
    guttered_position_y,
    (guttered_position_x + corner->x) - (guttered_position_x + outer_radius),
    outer_radius
  );

  Region* sweep_south = new Region(
    guttered_position_x,
    guttered_position_y + outer_radius,
    guttered_position_x + corner->x - gutter,
    sweep_base_height
  );

  Region* sweep_shelf = new Region(
      guttered_position_x + corner->x, guttered_position_y,
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
    corner->x + inner_radius + gutter,
    guttered_position_y,
    bounds->size->x,
    corner->y - gutter
  );

  Region* side_bar = new Region(
    guttered_position_x,
    guttered_position_y + outer_radius + sweep_base_height + gutter,
    corner->x - gutter,
    bounds->size->y - outer_radius - gutter - gutter
  );

  side_bar->Draw(renderer, colour);
  top_bar->Draw(renderer, colour);


  int rix = corner->x + inner_radius,
      riy = corner->y + inner_radius;

  boxColor(renderer,
    corner->x,
    corner->y,
    corner->x + inner_radius,
    corner->y + inner_radius,
    colour
  );

  filledCircleColor(renderer,
    rix+1, riy+1,
    inner_radius,
    0xff000000
  );

  corner->Draw(renderer, 0xffffffff);

  delete top_bar, side_bar;
}

Region* Elbo::ContainerRegion() {
  return new Region(
    corner->x + gutter,
    corner->y + gutter + inner_radius + sweep_base_height,
    bounds->size->x - (corner->x + gutter),
    bounds->size->y - (corner->y + gutter)
  );
}
