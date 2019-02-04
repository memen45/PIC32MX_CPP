#include "Clc.hpp"

enum {
CLCXCON_SPACING = 32, //spacing in words
CLC1CON = 0xBF802480,
    ON = 15,
    SIDL = 13,
    INTP = 11,
    INTN = 10,
    LCOE = 7,
    LCOUT = 6,
    LCPOL = 5,
CLCXSEL = 4, //offsets in words
CLCXGLS = 8, //offsets in words
};

//=============================================================================
            Clc::
Clc         (CLCX e)
            : m_clcx_con((volatile uint32_t*)CLC1CON + (e * CLCXCON_SPACING))
            {
            }

//=============================================================================
            auto Clc::
gate_inv    (GXPOL e, bool tf) -> void
            {
            setbit(m_clcx_con, e, tf);
            }

//=============================================================================
            auto Clc::
on          (bool tf) -> void
            {
            setbit(m_clcx_con, 1<<ON, tf);
            }

//=============================================================================
            auto Clc::
stop_idle   (bool tf) -> void
            {
            setbit(m_clcx_con, 1<<SIDL, tf);
            }

//=============================================================================
            auto Clc::
intp        (bool tf) -> void
            {
            setbit(m_clcx_con, 1<<INTP, tf);
            }

//=============================================================================
            auto Clc::
intn        (bool tf) -> void
            {
            setbit(m_clcx_con, 1<<INTN, tf);
            }

//=============================================================================
            auto Clc::
out         (bool tf) -> void
            {
            setbit(m_clcx_con, 1<<LCOE, tf);
            }

//=============================================================================
            auto Clc::
out         () -> bool
            {
            return anybit(m_clcx_con, 1<<LCOUT);
            }

//=============================================================================
            auto Clc::
out_inv     (bool tf) -> void
            {
            setbit(m_clcx_con, 1<<LCPOL, tf);
            }

//=============================================================================
            auto Clc::
mode        (MODE e) -> void
            {
            clrbit(m_clcx_con, LSR);
            setbit(m_clcx_con, e);
            }

//input select (no change if v invalid value)
//=============================================================================
            auto Clc::
in_sel      (SELX e, uint8_t v) -> void
            {
            if(v > 7) return;
            uint8_t n = e << 2;
            clrbit(m_clcx_con + CLCXSEL, 7<<n);
            setbit(m_clcx_con + CLCXSEL, v<<n);
            }

//or precomputed value for all gates
//=============================================================================
            auto Clc::
in_sel      (uint32_t v) -> void
            {
            val(m_clcx_con + CLCXSEL, v);
            }

//gate select setting fo one gate
//=============================================================================
            auto Clc::
gate_sel    (GLSX e, uint8_t v) -> void
            {
            uint8_t n = e << 3;
            clrbit(m_clcx_con + CLCXGLS, 15<<n);
            setbit(m_clcx_con + CLCXGLS, v<<n);
            }

//or precomputed register value for all gates
//=============================================================================
            auto Clc::
gate_sel    (uint32_t v) -> void
            {
            val(m_clcx_con + CLCXGLS, v);
            }
