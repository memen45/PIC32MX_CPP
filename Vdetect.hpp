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
        V325_363 = 5,
        V295_330,
        V275_309,
        V265_298,
        V245_280,
        V235_269,
        V225_255,
        V215_244,
        V208_233,
        V200_222,
        VEXT
    };
    static void     limit           (HLVDL);
};
