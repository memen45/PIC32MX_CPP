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

    static uint8_t  flags               ();                 //get all
    static bool     flag                (FLAGSOTG);         //get one
    static void     flags_clear         ();                 //clear all
    static void     flags_clear         (uint8_t);          //clear one or more
    static void     flag_clear          (FLAGSOTG);         //clear one

    static uint16_t irqs                ();                 //get all
    static bool     irq                 (FLAGSOTG);         //get one
    static void     irqs_clear          ();                 //clear all
    static void     irqs_clear          (uint8_t);          //clr one or more
    static void     irq_clear           (FLAGS);            //clr one

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

uint8_t Usb::flags(){ return (Usb::FLAGSOTG)Reg::val(U1OTGIR); } //get all
bool Usb::flag(FLAGSOTG e){ return Reg::anybit(U1OTGIR, e); } //get one
void Usb::flags_clear(){ Reg::val(U1OTGIR, 255); } //clear all
void Usb::flags_clear(uint8_t v){ Reg::val(U1OTGIR, v); } //clear one or more
void Usb::flag_clear(FLAGSOTG e){ Reg::val(U1OTGIR, e); } //clear one

uint8_t Usb::irqs(){ return Reg::val(U1OTGIR); } //get all
bool Usb::irq(FLAGSOTG e){ return Reg::anybit(U1OTGIR, e); } //get one
void Usb::irqs_clear(){ Reg::val(U1OTGIR, 255); } //clear all
void Usb::irqs_clear(uint8_t v){ Reg::val(U1OTGIR, v); } //clear one or more
void Usb::irq_clear(FLAGSOTG e){ Reg::val(U1OTGIR, e); } //clear one

uint8_t Usb::stat(){ return Reg::val(U1OTGSTAT); } //get all
bool Usb::stat(FLAGSOTG e){ return Reg::anybit(U1OTGSTAT, e); } //get one

void Usb::otg(OTG e, bool tf){ Reg::set(U1OTGCON, e, tf); }

void Usb::low_speed(bool tf){ Reg::set(U1ADDR, LSEN, tf); }

uint8_t Usb::tok_pid(){ return (Usb::TOKPID)(Reg::val(U1TOK)>>4); }
uint8_t Usb::tok_ep(){ return Reg::val(U1TOK) & 15; }
void Usb::tok_ep(uint8_t v){ Reg::clr(U1TOK, 15); Reg::set(U1TOK, v & 15); }

void Usb::sof_cnt(SOFVALS e){ Reg::val(U1SOF, e); }