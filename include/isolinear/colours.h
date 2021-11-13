#pragma once

typedef u_int32_t Colour;

class ColourScheme {
  public:
    Colour background{0x33000000};
    Colour disabled;
    Colour dark;
    Colour frame;
    Colour light;
    Colour light_alternate;
    Colour active;

    ColourScheme()
      : ColourScheme{
          0x33ffffff, 0x33ffffff,
          0x33ffffff, 0x33ffffff,
          0x33ffffff, 0x33ffffff
        }
    {};

    ColourScheme(
        Colour di,
        Colour d,
        Colour f,
        Colour l,
        Colour la,
        Colour a
      ) :
        background{0xff000000},
        disabled{di},
        dark{d},
        frame{f},
        light{l},
        light_alternate{la},
        active{a}
    {};

};


ColourScheme debug_colours{};

ColourScheme nightgazer_colours{
    0xff302829, 0xff41596f,
    0xff4582c6, 0xff6b7dca,
    0xff65b0c7, 0xff93e0e2
  };

ColourScheme blue_alert_colours{
    0xff4d4d4d, 0xffcc0000,
    0xffff3300, 0xffff6601,
    0xffff5942, 0xfffe9b8d
  };

ColourScheme yellow_alert_colours{
    0xff4d4d4d, 0xff054466,
    0xff0664a6, 0xff0d87cd,
    0xff0ea9ff, 0xff66d9fe
  };

ColourScheme red_alert_colours{
    0xff4d4d4d, 0xff000066,
    0xff000199, 0xff0100cc,
    0xff0000fe, 0xff9091ff
  };

