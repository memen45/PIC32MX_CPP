#pragma once

#include "Syskey.hpp"
#include "Reg.hpp"
#include "Irq.hpp"

/*=============================================================================
 Resets functions
=============================================================================*/

struct Resets {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //reset cause
    enum CAUSE : uint8_t {
        EXTR = 1<<7, SWR = 1<<6, //1<<5 none- reads 0
        WDTO = 1<<4, SLEEP = 1<<3, IDLE = 1<<2, BOR = 1<<1, POR = 1<<0
    };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //RCON
    static CAUSE    cause           ();
    static bool     config_err      ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //RSWRST
    static void     swreset         ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //RNMICON
    static bool     nmi_wdt         ();
    static bool     nmi_sw          ();
    static bool     nmi_gen         ();
    static bool     nmi_clkf        ();
    static bool     nmi_wdts        ();
    static void     nmi_wdtcount    (uint16_t);
    static void     nmi_wdtclr      ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //PWRCON
    static void     bor             (bool);
    static void     reten           (bool);
    static void     vregs           (bool);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;
    static Syskey sk;

    enum {
        RCON = 0xBF8026E0,
            PORIO = 1<<31, PORCORE = 1<<30,
            BCFGERR = 1<<27, BCFGFAIL = 1<<26, CMR = 1<<9,
        RSWRST = 0xBF8026F0,
            SWRST = 1,
        RNMICON = 0xBF802700,
            WDTR = 1<<24, SWNMI = 1<<23, GNMI = 1<<19, CF = 1<<17, WDTS = 1<<16,
        PWRCON = 0xBF802710,
            SBOREN = 1<<2, RETEN = 1<<0, VREGS = 1<<0
    };
};

/*=============================================================================
 all functions inline
=============================================================================*/
//save rcon on boot (only one time)
static uint32_t boot_flags;

//RCON
//save flags, clear flags, return reset cause
auto Resets::cause() -> CAUSE {
    //save flags only if not already saved
    //boot_flags var will be 0 on any reset as c runtime will clear
    //before this function can run
    if(boot_flags == 0){
        boot_flags = r.val(RCON);  //save
        r.val(RCON, 0);            //then clear all flags (for next reset)
    }
    //check for por first- specific combo
    if(boot_flags == (PORIO|PORCORE|BOR|POR)) return POR;
    //then go through flags high bits to low bits
    //(sleep is before idle, so will not get false flag
    // because sleep also has idle set)
    uint8_t ret = EXTR;
    for(; ret > POR; ret >>= 1){
        if(boot_flags & ret) break;
    }
    return (CAUSE)ret;
}
//check if config bits error
bool Resets::config_err(){
    return boot_flags & (BCFGERR|BCFGFAIL|CMR);
}
//RSWRST
//resets after read
void Resets::swreset(){
    Irq::disable_all();
    sk.unlock();
    r.setbit(RSWRST, SWRST);
    r.val(RSWRST);
    while(1);
}
//RNMICON
bool Resets::nmi_wdt(){
    return r.anybit(RNMICON, WDTR);
}
bool Resets::nmi_sw(){
    return r.anybit(RNMICON, SWNMI);
}
bool Resets::nmi_gen(){
    return r.anybit(RNMICON, GNMI);
}
bool Resets::nmi_clkf(){
    return r.anybit(RNMICON, CF);
}
bool Resets::nmi_wdts(){
    return r.anybit(RNMICON, WDTS);
}
void Resets::nmi_wdtcount(uint16_t v){
    r.val(RNMICON, v);
}
void Resets::nmi_wdtclr(){
    r.setbit(RNMICON, WDTR, 0);
}
//PWRCON
void Resets::bor(bool tf){
    sk.unlock();
    r.setbit(PWRCON, SBOREN, tf);
    sk.lock();
}
void Resets::reten(bool tf){
    sk.unlock();
    r.setbit(PWRCON,RETEN, tf);
    sk.lock();
}
void Resets::vregs(bool tf){
    sk.unlock();
    r.setbit(PWRCON,VREGS, tf);
    sk.lock();
}