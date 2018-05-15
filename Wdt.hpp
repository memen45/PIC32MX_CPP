#pragma once

//Watchdog timer

#include <cstdint>

struct Wdt {

        static auto
    reset () -> void;

        static auto
    on (bool) -> void;

        static auto
    window_on (bool) -> void;

};
