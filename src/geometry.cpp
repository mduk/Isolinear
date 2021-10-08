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
  int nearx = this->position.x,
      neary = this->position.y,
       farx = this->position.x + this->size.x,
       fary = this->position.y + this->size.y;

  return ( nearx <= point.x )
      && ( point.x <= farx  )
      && ( neary <= point.y )
      && ( point.y <= fary  );
}
