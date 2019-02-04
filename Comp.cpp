#include "Comp.hpp"

enum {
CMSTAT = 0xBF802300, //same for all
    SIDL = 13,
    CVREFSEL = 8,
CM1CON = 0xBF802310, CMXCON_SPACING = 8, //spacing in words
    ON = 15,
    COE = 14,
    CPOL = 13,
    CEVT = 9,
    COUT = 8,
    EVPOL_SHIFT = 6, EVPOL_CLR = 3,
    CREF = 4
};

//=============================================================================
            Comp::
Comp        (CMX e)
            : m_cmpx_con((volatile uint32_t*)CM1CON + (e * CMXCON_SPACING))
            {
            }

//=============================================================================
            auto Comp::
on          (bool tf) -> void
            {
            setbit(m_cmpx_con, 1<<ON, tf);
            }

//=============================================================================
            auto Comp::
out         (bool tf) -> void
            {
            setbit(m_cmpx_con, 1<<COE, tf);
            }

//=============================================================================
            auto Comp::
out_inv     (bool tf) -> void
            {
            setbit(m_cmpx_con, 1<<CPOL, tf);
            }

//=============================================================================
            auto Comp::
evt_bit     () -> bool
            {
            return anybit(m_cmpx_con, 1<<CEVT);
            }

//=============================================================================
            auto Comp::
out_bit     () -> bool
            {
            return anybit(m_cmpx_con, 1<<COUT);
            }

//=============================================================================
            auto Comp::
evt_sel     (EVPOL e) -> void
            {
            clrbit(m_cmpx_con, EVPOL_CLR<<EVPOL_SHIFT);
            setbit(m_cmpx_con, e<<EVPOL_SHIFT);
            }

//=============================================================================
            auto Comp::
cref_cxina  (bool tf) -> void
            {
            setbit(m_cmpx_con, 1<<CREF, not tf);
            }

//=============================================================================
            auto Comp::
ch_sel      (CCH e) -> void
            {
            clrbit(m_cmpx_con, BGAP);
            setbit(m_cmpx_con, e);
            }

//common static functions
//=============================================================================
            auto Comp::
stop_idle   (bool tf) -> void
            {
            setbit(CMSTAT, 1<<SIDL, tf);
            }

//=============================================================================
            auto Comp::
cref_sel    (CVREF e) -> void
            {
            setbit(CMSTAT, 1<<CVREFSEL, e);
            }
