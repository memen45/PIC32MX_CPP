#pragma once

//Timer1 functions

#include <cstdint>

struct Timer1 {

            static auto
timer       (uint16_t) -> void;

            static auto
timer       () -> uint16_t;

            static auto
period      (uint16_t) -> void;

            static auto
period      () -> uint16_t;

            static auto
on          (bool) -> void;

            static auto
stop_idle   (bool) -> void;

            static auto
wr_async    (bool) -> void;

            static auto
wr_busy     () -> bool;

            enum
CLK         : uint16_t {
            //bit1 = sysclock/ext, bit8:7 = ext src, 0=sosc,1=t1ck,2=lprc
            PBCLK = 0, SOSC = 1<<1, T1CK = 1<<8|1<<1, LPRC = 2<<8|1<<1
            };

            //clock source - also sets TCS unless pbclk
            static auto
clk_src     (CLK) -> void;

            static auto
tgate       (bool) -> void;

            enum
TCKPS       : uint8_t { PS1, PS8, PS64, PS256 };

            static auto
prescale    (TCKPS) -> void;

            static auto
tsync       (bool) -> void;

            //for Osc- so it can backup/restore T1CON if it needs
            //timer1 to check ext osc speed
            static auto
t1con       () -> uint16_t;

            static auto
t1con       (uint16_t) -> void;

};
