#pragma once

/*=============================================================================
 USB Host
=============================================================================*/

#include <cdtdint>
#include "Reg.hpp"

class UsbOTG {

    public:

    enum FLAGSOTG : uint8_t {
        //U1OTGIR, U1OTGIE, U1OTGSTAT
        ID = 1<<7,
        MSTIMER = 1<<6, /*no STAT*/
        LINE_STABLE = 1<<5,
        ACTIVITY = 1<<4, /*no STAT*/
        SESSION = 1<<3, BVBUS = 1<<2, AVBUS = 1<<0
    };
    static FLAGSOTG flags               ();
    static bool     flag                (FLAGSOTG);
    static void     flag_clear          (FLAGSOTG);
    static void     flags_clear         ();
    static void     irq                 (FLAGSOTG, bool);
    static bool     stat                (FLAGSOTG);



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
    static TOKPID   tok_pid             ();
    static uint8_t  tok_ep              ();
    static void     tok_ep              (uint8_t);


        //U1SOF
    enum SOFVALS : uint8_t {
        SOF64 = 74, SOF32 = 42, SOF16 = 26, SOF8 = 18
    };
    static void     sof_cnt             (SOFVALS);



    private:

    //registers - all use only first 8bits
    enum {
        U1OTGIR = 0xBF808440,   //no SET, INV - all bits write-1-to-clear
        U1OTGIE = 0xBF808450,
        U1OTGSTAT = 0xBF808460, //no SET, INV, CLR
        U1OTGCON = 0xBF808470,
        U1ADDR = 0xBF808660,
            LSEN = 1<<7,
        U1TOK = 0xBF8086A0,
        U1SOF = 0xBF8086B0,
     };
};


/*=============================================================================
 all functions inline
=============================================================================*/

Usb::FLAGSOTG Usb::flags(){ (Usb::FLAGSOTG)Reg::val8(U1OTGIR); }
bool Usb::flag(FLAGSOTG e){ return Reg::is_set8(U1OTGIR, e); }
void Usb::flag_clear(FLAGSOTG e){ Reg::val8(U1OTGIR, 1); } //1 to clear
void Usb::flag_clear(){ Reg::val8(U1OTGIR, 255); } //clear all
void Usb::irq(FLAGSOTG e, bool tf){ Reg::set(U1OTGIR, e, tf); }
bool Usb::stat(FLAGSOTG e){ return Reg::is_set8(U1OTGSTAT, e); }
void Usb::otg(OTG e, bool tf){ Reg::set(U1OTGCON, e, tf); }

void Usb::low_speed(bool tf){ Reg::set(U1ADDR, LSEN, tf); }

Usb::TOKPID Usb::tok_pid(){ return (Usb::TOKPID)(Reg::val8(U1TOK)>>4); }
uint8_t Usb::tok_ep(){ return Reg::val8(U1TOK) & 15; }
void Usb::tok_ep(uint8_t v){ Reg::clr(U1TOK, 15); Reg::set(U1TOK, v & 15); }

void Usb::sof_cnt(SOFVALS e){ Reg::val8(U1SOF, e); }