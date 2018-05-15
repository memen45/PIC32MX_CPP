#pragma once

//Timer1 functions

#include <cstdint>

struct Timer1 {

        static auto
    timer (uint16_t) -> void;

        static auto
    timer () -> uint16_t;

        static auto
    period (uint16_t) -> void;

        static auto
    period () -> uint16_t;

        static auto
    on (bool) -> void;

        static auto
    stop_idle (bool) -> void;

        static auto
    wr_async (bool) -> void;

        static auto
    wr_busy () -> bool;

    //clock source - also sets TCS unless pbclk
        enum
    CLK : uint16_t {
        SOSC = 0x0002, T1CK = 0x0102, LPRC = 0x0202, PBCLK = 0x0000
    };
        static auto
    clk_src (CLK) -> void;

        static auto
    tgate (bool) -> void;

        enum
    TCKPS : uint8_t { PS1, PS8, PS64, PS256 };
        static auto
    prescale (TCKPS) -> void;

        static auto
    tsync (bool) -> void;

    //for Osc- so it can backup/restore T1CON if it needs
    //timer check ext osc speed
        static auto
    t1con () -> uint16_t;

        static auto
    t1con (uint16_t) -> void;

};
