#pragma once

/*=============================================================================
 Timer1 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Osc.hpp"

struct Timer1 {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //clock prescale values
    enum TCKPS : uint8_t { PS1 = 0, PS8, PS64, PS256 };

    //clock source - also sets TCS unless pbclk
    enum CLK : uint16_t {
        SOSC = 0x0002, T1CK = 0x0102, LPRC = 0x0202, PBCLK = 0x0000
    };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
            CLK_CLR = (3<<8)|(1<<1),
        TMR1 = 0xBF808010,
        PR1 = 0xBF808020
    };

};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//  functions inline
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Timer1::timer(uint16_t n){
    r.val(TMR1, n);
}
uint16_t Timer1::timer(){
    return r.val16(TMR1);
}
void Timer1::period(uint16_t n){
    r.val(PR1, n);
}
uint16_t Timer1::period(){
    return r.val16(PR1);
}
void Timer1::on(bool tf){
    r.setbit(T1CON, ON, tf);
}
void Timer1::stop_idle(bool tf){
    r.setbit(T1CON, SIDL, tf);
}
void Timer1::wr_async(bool tf){
    r.setbit(T1CON, TWDIS, !tf);
}
bool Timer1::wr_busy(){
    return r.anybit(T1CON, TWIP);
}
void Timer1::clk_src(CLK e){
    if(e == SOSC) Osc::sosc(true);
    r.clrbit(T1CON, CLK_CLR);
    r.setbit(T1CON, e);
}
void Timer1::tgate(bool tf){
    r.setbit(T1CON, TGATE, tf);
}
void Timer1::prescale(TCKPS e){
    r.clrbit(T1CON, TCKPS_CLR<<TCKPS_SHIFT);
    r.setbit(T1CON, e<<TCKPS_SHIFT);
}
void Timer1::tsync(bool tf){
    r.setbit(T1CON, TSYNC, tf);
}
