#pragma once

/*=============================================================================
 Timer1 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Timer1 {

    public:

    //clock prescale values
    enum PRESCALE { PS256 = 3<<4, PS64 = 2<<4, PS8 = 1<<4, PS1 = 0<<4 };
    //clock source
    enum CLK { EXT_SOSC = 2, EXT_T1CK = 258, EXT_LPRC = 514, INT_PBCLK = 0 };

    //public functions
    static void         timer           (uint16_t n);
    static uint16_t     timer           ();
    static void         period          (uint16_t n);
    static uint16_t     period          ();
    static void         on              (bool tf);
    static void         stop_idle       (bool tf);
    static void         wr_async        (bool tf);
    static bool         wr_busy         ();
    static void         clk_src         (CLK);
    static void         tgate           (bool tf);
    static void         prescale        (PRESCALE);
    static void         tsync           (bool tf);

    private:

    enum {
        T1CON = 0xBF808000, TMR1 = 0xBF808010, PR1 = 0xBF808020,
        ON = 1<<15, SIDL = 1<<13, TWDIS = 1<<12, TWIP = 1<<11,
        TGATE = 1<<7, TSYNC = 1<<2, CLK_CLR = (3<<8)|(1<<1)
    };

};

/*=============================================================================
 all functions inline
=============================================================================*/

void Timer1::timer(uint16_t n){ Reg::val(TMR1, n); }
uint16_t Timer1::timer(){ return Reg::val(TMR1); }
void Timer1::period(uint16_t n){ Reg::val(PR1, n); }
uint16_t Timer1::period(){ return Reg::val(PR1); }
void Timer1::on(bool tf){ Reg::set(T1CON, ON, tf); }
void Timer1::stop_idle(bool tf){ Reg::set(T1CON, SIDL, tf); }
void Timer1::wr_async(bool tf){ Reg::set(T1CON, TWDIS, !tf); }
bool Timer1::wr_busy(){ return Reg::is_set(T1CON, TWIP); }
void Timer1::clk_src(CLK e){ Reg::clr(T1CON, CLK_CLR); Reg::set(T1CON, e); }
void Timer1::tgate(bool tf){ Reg::set(T1CON, TGATE, tf); }
void Timer1::prescale(PRESCALE e){ Reg::clr(T1CON, PS256); Reg::set(T1CON, e); }
void Timer1::tsync(bool tf){ Reg::set(T1CON, TSYNC, tf); }
