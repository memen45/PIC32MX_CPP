#pragma once

/*=============================================================================
 Oscillator
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"
#include "Irq.hpp"

struct Osc {

    //public

    //osccon
    enum DIVS : uint8_t { //upper byte of osccon, spllcon
        DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV256
    };
    enum CNOSC : uint8_t { //cosc/nosc in scond byte of osccon
         LPRC = 5, SOSC = 4, POSC = 2, SPLL = 1, FRC = 0
    };

    static void     frcdiv      (DIVS);     //set
    static DIVS     frcdiv      ();         //get
    static CNOSC    source      ();         //get current osc source
    static void     source      (CNOSC);    //start switch to nosc
    static bool     locked      ();         //get clock locked status
    static void     sleep       (bool);     //sleep enable
    static bool     failed      ();         //clock failed?
    static void     sosc        (bool);     //sosc enable


    //spllcon
    enum PLLMUL : uint8_t {
        MUL2 = 0, MUL3, MUL4, MUL6, MUL8, MUL12, MUL24
    };

    static void     plldiv      (DIVS);     //set pll divider
    static DIVS     plldiv      ();         //get pll divider
    static void     pllmul      (PLLMUL);   //set pll multiplier
    static PLLMUL   pllmul      ();         //get pll multiplier
    static bool     pllfrc      ();         //true=pll src frc, false=posc

    /*
        current speed =
            if source()==LPRC then 32,000
                       ==SOSC then assume 32768
                       ==POSC then unable
                       ==SPLL then more checking needed
                       ==FRC  then more checking needed

    */





    private:

    static Reg r;
    static Syskey sk;
    static Irq ir;

    enum {
        OSCCON = 0xBF802680,
            COSC_SHIFT = 4, //byte accessed via osccon[1]
            CLKLOCK = 1<<7, SLPEN = 1<<5, CF = 1<<3,
            SOSCEN = 1<<1, OSWEN = 1,
        SPLLCON = 0xBF8026A0,
            PLLICLK = 1<<7,
        REFO1CON = 0xBF802720,
        REFO1TRIM = 0xBF802730,
        CLKSTAT = 0xBF802770,
        OSCTUN = 0xBF802880
    };



};

/*=============================================================================
 all functions inline
=============================================================================*/
void Osc::frcdiv(DIVS e){
    sk.unlock();
    r.val(OSCCON+3, e); //upper byte access (only FRCDIV in upper byte)
    sk.lock();
}
Osc::DIVS Osc::frcdiv(){
    return (DIVS)r.val8(OSCCON+3);
}
Osc::CNOSC Osc::source(){
    return (CNOSC)(r.val8(OSCCON+1)>>4);
}
void Osc::source(CNOSC e){
    bool irstatus = ir.all_ison();
    ir.disable_all();
    sk.unlock();
    r.val(OSCCON+1, e); //can't over-write cosc, so write whole byte
    r.setbit(OSCCON, OSWEN);
    while(r.anybit(OSCCON, OSWEN));
    sk.lock();
    if(irstatus) ir.enable_all();
}
bool Osc::locked(){
    return r.anybit(OSCCON, CLKLOCK);
}
void Osc::sleep(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SLPEN, tf);
    sk.lock();
}
bool Osc::failed(){
    return r.anybit(OSCCON, CF);
}
void Osc::sosc(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SOSCEN, tf);
    sk.lock();
}
void Osc::plldiv(DIVS e){
    sk.unlock();
    r.val(SPLLCON+3, e);
    sk.lock();
}
void Osc::pllmul(PLLMUL e){
    sk.unlock();
    r.val(SPLLCON+2, e);
    sk.lock();
}
Osc::DIVS Osc::plldiv(){
    return (DIVS)r.val(SPLLCON+3);
}
Osc::PLLMUL Osc::pllmul(){
    return (PLLMUL)r.val8(SPLLCON+2);
}
bool Osc::pllfrc(){
    return r.anybit(SPLLCON, PLLICLK);
}