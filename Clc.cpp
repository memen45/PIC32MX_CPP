#include "Clc.hpp"
#include "Reg.hpp"

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
            Reg::setbit(m_clcx_con, e, tf);
            }

//=============================================================================
            auto Clc::
on          (bool tf) -> void
            {
            Reg::setbit(m_clcx_con, 1<<ON, tf);
            }

//=============================================================================
            auto Clc::
stop_idle   (bool tf) -> void
            {
            Reg::setbit(m_clcx_con, 1<<SIDL, tf);
            }

//=============================================================================
            auto Clc::
intp        (bool tf) -> void
            {
            Reg::setbit(m_clcx_con, 1<<INTP, tf);
            }

//=============================================================================
            auto Clc::
intn        (bool tf) -> void
            {
            Reg::setbit(m_clcx_con, 1<<INTN, tf);
            }

//=============================================================================
            auto Clc::
out         (bool tf) -> void
            {
            Reg::setbit(m_clcx_con, 1<<LCOE, tf);
            }

//=============================================================================
            auto Clc::
out         () -> bool
            {
            return Reg::anybit(m_clcx_con, 1<<LCOUT);
            }

//=============================================================================
            auto Clc::
out_inv     (bool tf) -> void
            {
            Reg::setbit(m_clcx_con, 1<<LCPOL, tf);
            }

//=============================================================================
            auto Clc::
mode        (MODE e) -> void
            {
            Reg::clrbit(m_clcx_con, LSR);
            Reg::setbit(m_clcx_con, e);
            }

//input select, n = 1-4, v = 0-7 (invalid args masked to good vals)
//=============================================================================
            auto Clc::
in_sel      (uint8_t n, uint8_t v) -> void
            {
            n -= 1; n and_eq 3; n <<= 2; v and_eq 7;
            Reg::clrbit(m_clcx_con + CLCXSEL, 7<<n);
            Reg::setbit(m_clcx_con + CLCXSEL, v<<n);
            }

//or all in in shot with precomputed value
//=============================================================================
            auto Clc::
in_sel      (uint32_t v) -> void
            {
            Reg::val(m_clcx_con + CLCXSEL, v);
            }

//gate select, n = 1-4 (invalid gate masked to good gate)
//=============================================================================
            auto Clc::
gate_sel    (uint8_t n, uint8_t v) -> void
            {
            n -= 1; n and_eq 3; n <<= 3;
            Reg::clrbit(m_clcx_con + CLCXGLS, 15<<n);
            Reg::setbit(m_clcx_con + CLCXGLS, v<<n);
            }

//or all in in shot with precomputed value
//=============================================================================
            auto Clc::
gate_sel    (uint32_t v) -> void
            {
            Reg::val(m_clcx_con+CLCXGLS, v);
            }
