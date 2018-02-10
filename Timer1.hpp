#pragma once

//Timer1 functions

#include <cstdint>
#include "Reg.hpp"

struct Timer1 {

    //clock prescale values
    enum TCKPS : uint8_t { PS1 = 0, PS8, PS64, PS256 };

    //clock source - also sets TCS unless pbclk
    enum CLK : uint16_t {
        SOSC = 0x0002, T1CK = 0x0102, LPRC = 0x0202, PBCLK = 0x0000
    };

    static void         timer           (uint16_t);
    static uint16_t     timer           ();
    static void         period          (uint16_t);
    static uint16_t     period          ();
    static void         on              (bool);
    static void         stop_idle       (bool);
    static void         wr_async        (bool);
    static bool         wr_busy         ();
    static void         clk_src         (CLK);
    static void         tgate           (bool);
    static void         prescale        (TCKPS);
    static void         tsync           (bool);

    //public so Osc:: can have it to backup when
    //it calculates ext clock freq
    enum { T1CON = 0xBF808000 };

    private:

    static Reg r;

    enum {
        //T1CON
            ON = 1<<15,
            SIDL = 1<<13,
            TWDIS = 1<<12,
            TWIP = 1<<11,
            TGATE = 1<<7,
            TCKPS_SHIFT = 4, TCKPS_CLR = 3,
            TSYNC = 1<<2,
            CLK_CLR = (3<<8) | (1<<1),
        TMR1 = 0xBF808010,
        PR1 = 0xBF808020
    };

};
