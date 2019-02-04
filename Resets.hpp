#pragma once

//Resets

#include <cstdint>
#include "Reg.hpp"

struct Resets : private Reg {

            //=== RCON ===

            enum
CAUSE       : uint8_t {
            EXTR = 1<<7, SWR = 1<<6, //1<<5 none- reads 0
            WDTO = 1<<4, SLEEP = 1<<3, IDLE = 1<<2, BOR = 1<<1, POR = 1<<0
            };

            static auto
cause       () -> CAUSE;

            static auto
config_err  () -> bool;


            //==== RSWRST ====

            static auto
swreset     () -> void;


            //==== RCON ====

            static auto
vregs       (bool) -> void;

};
