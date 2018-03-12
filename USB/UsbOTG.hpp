#pragma once

//USB Host

#include <cstdint>

struct UsbOTG {

    enum FLAGSOTG : uint8_t {
        //U1OTGIR, U1OTGIE, U1OTGSTAT
        ID = 1<<7,
        MSTIMER = 1<<6, /*no STAT*/
        LINE_STABLE = 1<<5,
        ACTIVITY = 1<<4, /*no STAT*/
        SESSION = 1<<3, BVBUS = 1<<2, AVBUS = 1<<0
    };

    static uint8_t  flags               ();                 //get all
    static bool     flag                (FLAGSOTG);         //get one
    static void     flags_clr           ();                 //clear all
    static void     flags_clr           (uint8_t);          //clear one or more
    static void     flag_clr            (FLAGSOTG);         //clear one

    static uint8_t  irqs                ();                 //get all
    static bool     irq                 (FLAGSOTG);         //get one
    static void     irqs_clr            ();                 //clear all
    static void     irqs_clr            (uint8_t);          //clr one or more
    static void     irq_clr             (FLAGSOTG);         //clr one

    static uint8_t  stat                ();                 //get all
    static bool     stat                (FLAGSOTG);         //get one

    enum OTG : uint8_t {
        //U1OTGCON
        DPUP = 1<<7, DMUP = 1<<6, DPDN = 1<<5, DMDN = 1<<4,
        VBUSON = 1<<3, OTGON = 1<<2, VCHARGE = 1<<1, VDISCHARGE = 1<<0
    };

    static void     otg                 (OTG, bool);

    //U1ADDR
    static void     low_speed           (bool);

    //U1TOK
    enum TOKPID : uint8_t { SETUP = 13, IN = 9, OUT = 1 };

    static uint8_t  tok_pid             ();
    static uint8_t  tok_ep              ();
    static void     tok_ep              (uint8_t);

    //U1SOF
    enum SOFVALS : uint8_t {
        SOF64 = 74, SOF32 = 42, SOF16 = 26, SOF8 = 18
    };

    static void     sof_cnt             (SOFVALS);

};

