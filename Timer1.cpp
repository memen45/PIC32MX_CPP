#include "Timer1.hpp"
#include "Osc.hpp"

enum {
    T1CON = 0xBF800600,
        ON = 15,
        SIDL = 13,
        TWDIS = 12,
        TWIP = 11,
        TGATE = 7,
        TCKPS_SHIFT = 4, TCKPS_MASK = 3,
        TSYNC = 2,
        TCS = 1,
    TMR1 = 0xBF800610,
    PR1 = 0xBF800620
};


//=============================================================================
            auto Timer1::
timer       (uint16_t n) -> void
            {
            val(TMR1, n);
            }

//=============================================================================
            auto Timer1::
timer       () -> uint16_t
            {
            return val16(TMR1);
            }

//=============================================================================
            auto Timer1::
period      (uint16_t n) -> void
            {
            val(PR1, n);
            }

//=============================================================================
            auto Timer1::
period      () -> uint16_t
            {
            return val16(PR1);
            }

//=============================================================================
            auto Timer1::
on          (bool tf) -> void
            {
            setbit(T1CON, 1<<ON, tf);
            }

//=============================================================================
            auto Timer1::
stop_idle   (bool tf) -> void
            {
            setbit(T1CON, 1<<SIDL, tf);
            }

//=============================================================================
            auto Timer1::
wr_async    (bool tf) -> void
            {
            setbit(T1CON, 1<<TWDIS, not tf);
            }

//=============================================================================
            auto Timer1::
wr_busy     () -> bool
            {
            return anybit(T1CON, 1<<TWIP);
            }

//=============================================================================
            auto Timer1::
clk_src     (CLK e) -> void
            {
            clrbit(T1CON, 1 << TCS);
            setbit(T1CON, 1 << TCS, e);
            }

//=============================================================================
            auto Timer1::
tgate       (bool tf) -> void
            {
            setbit(T1CON, 1<<TGATE, tf);
            }

//=============================================================================
            auto Timer1::
prescale    (TCKPS e) -> void
            {
            clrbit(T1CON, TCKPS_MASK<<TCKPS_SHIFT);
            setbit(T1CON, e<<TCKPS_SHIFT);
            }

//=============================================================================
            auto Timer1::
tsync       (bool tf) -> void
            {
            setbit(T1CON, 1<<TSYNC, tf);
            }

//=============================================================================
            auto Timer1::
t1con       () -> uint16_t
            {
            return val16(T1CON);
            }

//=============================================================================
            auto Timer1::
t1con       (uint16_t v) -> void
            {
            val(T1CON, v);
            }
