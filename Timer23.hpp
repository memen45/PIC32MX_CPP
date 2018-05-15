#pragma once

//Timer2/Timer3

#include <cstdint>

struct Timer23 {

    //instantiate with timer number
        enum
    TMR23 { TMR2, TMR3 };
    Timer23(TMR23);

        auto
    count (uint32_t) -> void;

        auto
    count () -> uint32_t;

        auto
    period (uint32_t) -> void;

        auto
    period () -> uint32_t;

        auto
    on (bool) -> void;

        auto
    stop_idle (bool) -> void;

        auto
    gate (bool) -> void;

        enum
    PRESCALE { PS1, PS2, PS4, PS8, PS16, PS32, PS64, PS256 };
        auto
    prescale (PRESCALE) -> void;

        auto
    mode32 (bool) -> void; //T2 only (harmless for T3)

        enum
    CLK { PBCLK, T2CK };
        auto
    clk_src (CLK) -> void;


    private:

    volatile uint32_t* m_txcon;
    volatile uint32_t& m_tmrx;
    volatile uint32_t& m_prx;

};
