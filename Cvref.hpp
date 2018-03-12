#pragma once

//Voltage Reference (CVref, DAC)

#include <cstdint>

struct Cvref {

    static void     on              (bool);
    static void     out             (bool);

    enum REFSEL { NONE, CVREF, AVDD };
    static void     refsel          (REFSEL);

    static void     dacdat          (uint8_t);

};
