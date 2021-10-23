#pragma once

#include <cstdlib>
#include <vector>

typedef u_int32_t Colour;

Colour RandomColour() {
  std::vector <Colour> colours {
    0xff3f403f, 0xff99ffff, 0xff66ccff, 0xff3399ff,
    0xff664466, 0xffcc99cc, 0xffffcc99, 0xffcc6633,
    0xff996600, 0xffcc9999, 0xffff9999, 0xffcc99cc,
    0xff9966cc, 0xff6666cc, 0xff6699ff, 0xff0099ff,
    0xff66ccff, 0xffcc8866, 0xffbb5544, 0xffaa7799,
    0xff664477, 0xff4466dd, 0xff3355aa, 0xff2266bb,
    0xff5599ee, 0xffffddcc, 0xffff9955, 0xffff6633,
    0xffee1100, 0xff880000, 0xff55aabb, 0xff1144bb,
    0xff112288
  };

  return colours[rand() % colours.size()];
}

class ColourScheme {
  public:
    Colour base;
    Colour active;
    Colour inactive;
    Colour disabled;

    ColourScheme(Colour b, Colour a, Colour i, Colour d)
      : base{b}, active{a}, inactive{i}, disabled{d}
    {};

};
