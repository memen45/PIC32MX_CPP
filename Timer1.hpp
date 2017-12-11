#pragma once

/*=============================================================================
 Timer1 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Timer1 {

    public:

    //public functions
    static void     timer           (uint16_t n);
    static uint16_t timer           (void);
    static void     period          (uint16_t n);
    static uint16_t period          (void);
    static void     on              (bool tf);
    static void     stop_idle       (bool tf);
    static void     async_wrdis     (bool tf);
    static bool     wr_busy         (void);
    static void     clk_sosc        (void);
    static void     clk_lprc        (void);
    static void     clk_t1ck        (void);
    static void     tgate_on        (bool tf);
    static void     pre_1           (void);
    static void     pre_256         (void);
    static void     pre_64          (void);
    static void     pre_8           (void);
    static void     tsync_on        (bool tf);
    static void     ext_clk         (bool tf);

    private:

    enum {
        T1CON = 0xBF808000,
        TMR1 =  0xBF808010,
        PR1 =   0xBF808020,
        ON = 1<<15,     SIDL = 1<<13,    TWDIS = 1<<12,   TWIP = 1<<11,
        EXT_RES = 3<<8, EXT_LPRC = 2<<8, EXT_T1CK = 1<<8, EXT_SOSC = 0<<8,
        TGATE = 1<<7,
        TCKPS_256 = 3<<4, TCKPS_64 = 2<<4, TCKPS_8 = 1<<4, TCKPS_1 = 0<<4,
        TSYNC = 1<<2,
        TCS = 1<<1
    };

};

/*=============================================================================
 all functions inline
=============================================================================*/

void Timer1::timer(uint16_t n){ Reg::val(TMR1, n); }
uint16_t Timer1::timer(void){ return Reg::val(TMR1); }
void Timer1::period(uint16_t n){ Reg::val(PR1, n); }
uint16_t Timer1::period(void){ return Reg::val(PR1); }
void Timer1::on(bool tf){ Reg::set(T1CON, ON, tf); }
void Timer1::stop_idle(bool tf){ Reg::set(T1CON, SIDL, tf); }
void Timer1::async_wrdis(bool tf){ Reg::set(T1CON, TWDIS, tf); }
bool Timer1::wr_busy(void){ return Reg::is_set(T1CON, TWIP); }
void Timer1::clk_sosc(void){ Reg::clr(T1CON, EXT_RES); }
void Timer1::clk_lprc(void){ clk_sosc(); Reg::set(T1CON, EXT_LPRC); }
void Timer1::clk_t1ck(void){ clk_sosc(); Reg::set(T1CON, EXT_T1CK) ; }
void Timer1::tgate_on(bool tf){ Reg::set(T1CON, TGATE, tf); }
void Timer1::pre_1(void){ Reg::clr(T1CON, TCKPS_256); }
void Timer1::pre_256(void){ Reg::set(T1CON, TCKPS_256); }
void Timer1::pre_64(void){ pre_1(); Reg::set(T1CON, TCKPS_64); }
void Timer1::pre_8(void){ pre_1(); Reg::set(T1CON, TCKPS_8); }
void Timer1::tsync_on(bool tf){ Reg::set(T1CON, TSYNC, tf); }
void Timer1::ext_clk(bool tf){ Reg::set(T1CON, TCS, tf); }