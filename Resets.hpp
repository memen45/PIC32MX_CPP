#pragma once

//Resets

#include <cstdint>

struct Resets {

    //RCON
    enum CAUSE : uint8_t {
        EXTR = 1<<7, SWR = 1<<6, //1<<5 none- reads 0
        WDTO = 1<<4, SLEEP = 1<<3, IDLE = 1<<2, BOR = 1<<1, POR = 1<<0
    };
    static CAUSE    cause           ();

    static bool     config_err      ();

    //RSWRST
    static void     swreset         ();

    //RNMICON
    static bool     nmi_wdt         ();
    static bool     nmi_sw          ();
    static bool     nmi_gen         ();
    static bool     nmi_clkf        ();
    static bool     nmi_wdts        ();
    static void     nmi_wdtcount    (uint16_t);
    static void     nmi_wdtclr      ();

    //PWRCON
    static void     bor             (bool);
    static void     reten           (bool); //used by Osc for reten sleep mode
    static void     vregs           (bool);

};
