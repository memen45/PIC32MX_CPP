#pragma once

//Resets

#include "Sys.hpp"
#include "Reg.hpp"

struct Resets {

    //reset cause
    enum CAUSE : uint8_t {
        EXTR = 1<<7, SWR = 1<<6, //1<<5 none- reads 0
        WDTO = 1<<4, SLEEP = 1<<3, IDLE = 1<<2, BOR = 1<<1, POR = 1<<0
    };

    //RCON
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

    private:

    static Reg r;
    static Sys sys;

    enum : uint32_t {
        RCON = 0xBF8026E0,
            PORIO = 1u<<31,
            PORCORE = 1<<30,
            BCFGERR = 1<<27,
            BCFGFAIL = 1<<26,
            CMR = 1<<9,
        RSWRST = 0xBF8026F0,
            SWRST = 1,
        RNMICON = 0xBF802700,
            WDTR = 1<<24,
            SWNMI = 1<<23,
            GNMI = 1<<19,
            CF = 1<<17,
            WDTS = 1<<16,
        PWRCON = 0xBF802710,
            SBOREN = 1<<2,
            RETEN = 1<<0,
            VREGS = 1<<0
    };

};
