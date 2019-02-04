#pragma once

//Watchdog timer

#include <cstdint>
#include "Reg.hpp"

struct Wdt : private Reg {

            static auto
reset       () -> void;

            static auto
on          (bool) -> void;

            static auto
window_on   (bool) -> void;

};
