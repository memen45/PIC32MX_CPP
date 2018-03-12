#pragma once

//Timer1 functions

#include <cstdint>

struct Timer1 {

    static void         timer           (uint16_t);
    static uint16_t     timer           ();
    static void         period          (uint16_t);
    static uint16_t     period          ();
    static void         on              (bool);
    static void         stop_idle       (bool);
    static void         wr_async        (bool);
    static bool         wr_busy         ();

    //clock source - also sets TCS unless pbclk
    enum CLK : uint16_t {
        PBCLK = 0, T1CK
    };
    static void         clk_src         (CLK);

    static void         tgate           (bool);

    enum TCKPS : uint8_t { PS1, PS8, PS64, PS256 };
    static void         prescale        (TCKPS);

    static void         tsync           (bool);

    //for Osc- so it can backup/restore T1CON if it needs
    //timer check ext osc speed
    static uint16_t     t1con           ();
    static void         t1con           (uint16_t);

};
