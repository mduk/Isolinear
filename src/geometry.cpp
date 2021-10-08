#include "geometry.h"




//////////////////////////////////////////////////
// Coordinate
//////////////////////////////////////////////////

void Coordinate::Add(Coordinate* c) {
  this->x += c->x;
  this->y += c->y;
}

void Coordinate::Subtract(Coordinate* c) {
  this->x -= c->x;
  this->y -= c->y;
}





//////////////////////////////////////////////////
// Region
//////////////////////////////////////////////////

bool Region::Encloses(Coordinate& point) {
  Position near = this->Near(),
           far  = this->Far();

  return ( near.x <= point.x )
      && ( near.y <= point.y )
      && ( point.x <= far.x  )
      && ( point.y <= far.y  );
}
