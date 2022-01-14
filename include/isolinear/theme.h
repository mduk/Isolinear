#pragma once

namespace isolinear::theme {


  typedef u_int32_t colour;

  class colour_scheme {
    public:
      colour background{0x33000000};
      colour disabled;
      colour dark;
      colour frame;
      colour light;
      colour light_alternate;
      colour active;

      colour white{0xffffffff};

      colour_scheme()
        : colour_scheme{
            0x33ffffff, 0x33ffffff,
            0x33ffffff, 0x33ffffff,
            0x33ffffff, 0x33ffffff
          }
      {};

      colour_scheme(
          colour di,
          colour d,
          colour f,
          colour l,
          colour la,
          colour a
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



  colour_scheme debug_colours{};

  colour_scheme nightgazer_colours{
      0xff302829, 0xff41596f,
      0xff4582c6, 0xff6b7dca,
      0xff65b0c7, 0xff93e0e2
    };

  colour_scheme blue_alert_colours{
      0xff4d4d4d, 0xffcc0000,
      0xffff3300, 0xffff6601,
      0xffff5942, 0xfffe9b8d
    };

  colour_scheme yellow_alert_colours{
      0xff4d4d4d, 0xff054466,
      0xff0664a6, 0xff0d87cd,
      0xff0ea9ff, 0xff66d9fe
    };

  colour_scheme red_alert_colours{
      0xff4d4d4d, 0xff000066,
      0xff000199, 0xff0100cc,
      0xff0000fe, 0xff9091ff
    };


}
