#pragma once

//Watchdog timer

#include <cstdint>

struct Wdt {

    static void     reset       ();
    static void     on          (bool);
    static void     window_on   (bool);

};
