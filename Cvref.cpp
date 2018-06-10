#include "Cvref.hpp"
#include "Reg.hpp"

enum {
DAC1CON = 0xBF802380,
    ON = 15,
    DACOE = 8,
    REFSEL_SHIFT = 0, REFSEL_CLR = 3
};

//=============================================================================
            auto Cvref::
on          (bool tf) -> void
            {
            Reg::setbit(DAC1CON, 1<<ON, tf);
            }

//=============================================================================
            auto Cvref::
out         (bool tf) -> void
            {
            Reg::setbit(DAC1CON, 1<<DACOE, tf);
            }

//=============================================================================
            auto Cvref::
refsel      (REFSEL e) -> void
            {
            Reg::clrbit(DAC1CON, REFSEL_CLR<<REFSEL_SHIFT);
            Reg::setbit(DAC1CON, e<<REFSEL_SHIFT);
            }

//dac level 0-31
//=============================================================================
            auto Cvref::
dacdat      (uint8_t v) -> void
            {
            Reg::val(DAC1CON + 2, v);
            }
