#pragma once

//High/Low-Voltage Detect

#include <cstdint>

struct Vdetect {

    static void     on              (bool);
    static void     stop_idle       (bool);
    static void     trip_above      (bool);
    static bool     bgap_stable     ();
    static bool     iref_stable     ();
    static bool     tripped         ();

    //voltage level limits
    enum HLVDL {
        VEXT = 15, V200_222 = 14, V208_233 = 13, V215_244 = 12, V225_255 = 11,
        V235_269 = 10, V245_280 = 9, V265_298 = 8, V275_309 = 7, V295_330 = 6,
        V325_363 = 5
    };
    static void     limit           (HLVDL);
};
