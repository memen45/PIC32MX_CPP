#pragma once

//Timer2/Timer3/Timer4/Timer5

#include <cstdint>
#include "Reg.hpp"

class Timer : private Reg {

    public:

            enum 
TMRX        { TMR2, TMR3, TMR4, TMR5 };
            //instantiate with timer number
Timer       (TMRX);

            auto
count       (uint32_t) -> void;

            auto
count       () -> uint32_t;

            auto
period      (uint32_t) -> void;

            auto
period      () -> uint32_t;

            auto
on          (bool) -> void;

            auto
stop_idle   (bool) -> void;

            auto
gate        (bool) -> void;

            enum
PRESCALE    { PS1, PS2, PS4, PS8, PS16, PS32, PS64, PS256 = 8 };

            auto
prescale    (PRESCALE) -> void;
            
            auto
prescale    () -> PRESCALE;

            auto
mode32      (bool) -> void; //T2 and T4 only (harmless for T3 and T5)

            enum
CLK         { PBCLK, TxCK };

            auto
clk_src     (CLK) -> void;


            private:

            volatile uint32_t* m_txcon;
            volatile uint32_t& m_tmrx;
            volatile uint32_t& m_prx;

};
