#pragma once

class Coordinate {
  public:
    unsigned int x, y;

    Coordinate(
        unsigned int x,
        unsigned int y
      );
    Coordinate* Copy();
    void Add(Coordinate* c);
    void Subtract(Coordinate* c);
};

class Position : public Coordinate {
  public:
    Position(
        unsigned int x,
        unsigned int y
      );
};

class Size : public Coordinate {
  public:
    Size(
        unsigned int w,
        unsigned int h
      );
};

class Region {
  public:
    Position* position;
    Size* size;

    Region(
        Position* p,
        Size* s
      );

    Region(
        unsigned int x,
        unsigned int y,
        unsigned int w,
        unsigned int h
      );

    int X() { return this->position->x; };
    int Y() { return this->position->y; };
    int W() { return this->size->x; };
    int H() { return this->size->y; };

    Region* Copy();
};
