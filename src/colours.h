#pragma once

typedef u_int32_t Colour;

class ColourScheme {
  public:
    Colour background{0x99000000};
    Colour base;
    Colour active;
    Colour inactive;
    Colour disabled;

    ColourScheme(Colour b, Colour a, Colour i, Colour d)
      : base{b}, active{a}, inactive{i}, disabled{d}
    {};

};
