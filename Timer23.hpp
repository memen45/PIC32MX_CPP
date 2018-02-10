#pragma once

//Timer2/Timer3

#include <cstdint>
#include "Reg.hpp"

class Timer23 {

    public:

    //instantiate with timer number
    enum TMR23 { TMR2 = 0, TMR3 };

    //prescale values
    enum PRESCALE { PS1 = 0, PS2, PS4, PS8, PS16, PS32, PS64, PS256 };

    //clock sources
    enum CLK { PBCLK = 0, T2CK };

    /*constexpr*/ Timer23(TMR23);

    void        count       (uint32_t) const;
    uint32_t    count       () const;
    void        period      (uint32_t) const;
    uint32_t    period      () const;
    void        on          (bool) const;
    void        stop_idle   (bool) const;
    void        gate        (bool) const;
    void        prescale    (PRESCALE) const;
    void        mode32      (bool) const; //T2 only (harmless for T3)
    void        clk_src     (CLK) const;

    private:

    static Reg r;

    enum {
        ON = 1<<15,
        SIDL = 1<<13,
        TGATE = 1<<7,
        TCKPS_SHIFT = 4, TCKPS_CLR = 7,
        T32 = 1<<3,
        TCS = 1<<1
    };

    enum { T1CON = 0xBF808040, TMR23_SPACING = 0x10 }; //spacing in words

    volatile uint32_t* m_txcon;
    volatile uint32_t& m_tmrx;
    volatile uint32_t& m_prx;
    
    using vu32ptr = volatile uint32_t*;

};
