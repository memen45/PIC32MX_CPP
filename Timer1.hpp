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
        SOSC = 0x0002, T1CK = 0x0102, LPRC = 0x0202, PBCLK = 0x0000
    };
    static void         clk_src         (CLK);

    static void         tgate           (bool);

    enum TCKPS : uint8_t { PS1 = 0, PS8, PS64, PS256 };
    static void         prescale        (TCKPS);

    static void         tsync           (bool);

    //public so Osc:: can have it to backup when
    //it calculates ext clock freq
    enum { T1CON = 0xBF808000 };

};
