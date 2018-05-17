#pragma once

//Resets

#include <cstdint>

struct Resets {

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


            //==== RNMICON ====

            static auto
nmi_wdt     () -> bool;

            static auto
nmi_sw      () -> bool;

            static auto
nmi_gen     () -> bool;

            static auto
nmi_clkf    () -> bool;

            static auto
nmi_wdts    () -> bool;

            static auto
nmi_wdtcount (uint16_t) -> void;

            static auto
nmi_wdtclr  () -> void;


            //==== PWRCON ====

            static auto
bor         (bool) -> void;

            //used by Osc for reten sleep mode
            static auto
reten       (bool) -> void;

            static auto
vregs       (bool) -> void;

};
