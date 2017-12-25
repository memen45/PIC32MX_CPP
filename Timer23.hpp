#pragma once

/*=============================================================================
 Timer2/Timer3 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Timer23 {

    public:

    //instantiate with timer number
    enum TMR23 { T2 = 0xBF808040, T3 = 0xBF808080 };
    //prescale values
    enum PRESCALE {
        PS256 = 7<<4, PS64 = 6<<4, PS32 = 5<<4, PS16 = 4<<4,
        PS8 = 3<<4, PS4 = 2<<4, PS2 = 1<<4, PS1 = 0<<4,
    };
    //clock sources
    enum CLK { EXT_T2CK = 1<<1, INT_PBCLK = 0 };

    //public functions
    void        timer       (uint16_t) const;
    uint16_t    timer       () const;
    void        pr          (uint16_t) const;
    uint16_t    pr          () const;
    void        on          (bool) const;
    void        stop_idle   (bool) const;
    void        tgate       (bool) const;
    void        prescale    (PRESCALE) const;
    void        t32bit      (bool) const; //T2 only (harmless for T3)
    void        clk_src     (CLK) const;

    //constructor
    constexpr Timer23(TMR23);

    private:

    static Reg r;

    enum {
        TMRX = 4, PRX = 8, //word offsets
        ON = 1<<15, SIDL = 1<<13,
        EXT_RES = 3<<8, EXT_LPRC = 2<<8, EXT_T1CK = 1<<8, EXT_SOSC = 0<<8,
        TGATE = 1<<7, T32 = 1<<3, TCS = 1<<1
    };

    //private vars
    volatile uint32_t * m_pt;
};

/*=============================================================================
 inline functions
=============================================================================*/

constexpr Timer23::Timer23(TMR23 e) : m_pt((volatile uint32_t*)e){}