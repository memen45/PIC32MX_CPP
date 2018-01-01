#pragma once

/*=============================================================================
 Timer1 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Timer1 {

    //clock prescale values
    enum PRESCALE { PS256 = 3<<4, PS64 = 2<<4, PS8 = 1<<4, PS1 = 0<<4 };
    //clock source
    enum CLK { EXT_SOSC = 2, EXT_T1CK = 258, EXT_LPRC = 514, INT_PBCLK = 0 };

    //public functions
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
    static void         prescale        (PRESCALE);
    static void         tsync           (bool);

    private:

    static Reg r;

    enum {
        T1CON = 0xBF808000, TMR1 = 0xBF808010, PR1 = 0xBF808020,
        ON = 1<<15, SIDL = 1<<13, TWDIS = 1<<12, TWIP = 1<<11,
        TGATE = 1<<7, TSYNC = 1<<2, CLK_CLR = (3<<8)|(1<<1)
    };

};

/*=============================================================================
 all functions inline
=============================================================================*/

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
    r.setbit(T1CON, CLK_CLR, 0);
    r.setbit(T1CON, e);
}
void Timer1::tgate(bool tf){
    r.setbit(T1CON, TGATE, tf);
}
void Timer1::prescale(PRESCALE e){
    r.setbit(T1CON, PS256, 0);
    r.setbit(T1CON, e);
}
void Timer1::tsync(bool tf){
    r.setbit(T1CON, TSYNC, tf);
}
