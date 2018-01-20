#pragma once

#include <cstdint>
#include "Reg.hpp"

//Compare 1/2/3

struct Comp {

    //instantiate Comp with comparator number
    enum CMX { CMP1 = 0, CMP2, CMP3 };

    //event polarity
    enum EVPOL { OFF = 0, LH, HL, ANY };

    //channel select
    enum CCH { CXINB = 0, CXINC, CXIND, BGAP };

    constexpr Comp(CMX);

    void            on              (bool);
    void            out             (bool);
    void            out_inv         (bool);
    bool            evt_bit         ();
    bool            out_bit         (void);
    void            evt_sel         (EVPOL);
    void            cref_cxina      (bool);
    void            ch_sel          (CCH);

    //common for all instances
    //cref source
    enum CVREF { INT_BGAP = 0, EXT_CVREF };

    static void     stop_idle       (bool);
    static void     cref_sel        (CVREF);

    private:

    static Reg r;

    enum {
        CMSTAT = 0xBF802300, //same for all
            SIDL = 1<<13,
            CVREFSEL = 1<<8,
        CM1CON = 0xBF802310, CMXCON_SPACING = 8, //spacing in words
            ON = 1<<15,
            COE = 1<<14,
            CPOL = 1<<13,
            CEVT = 1<<9,
            COUT = 1<<8,
            EVPOL_SHIFT = 6, EVPOL_CLR = 3,
            CREF = 1<<4
    };

    volatile uint32_t* m_cmpx_con;

};

//=============================================================================
    constexpr   Comp::Comp              (CMX e)
//=============================================================================
    : m_cmpx_con((volatile uint32_t*)CM1CON+(e*CMXCON_SPACING))
{
}

//=============================================================================
    void        Comp::on                (bool tf)
//=============================================================================
{
    r.setbit(m_cmpx_con, ON, tf);
}

//=============================================================================
    void        Comp::out               (bool tf)
//=============================================================================
{
    r.setbit(m_cmpx_con, COE, tf);
}

//=============================================================================
    void        Comp::out_inv           (bool tf)
//=============================================================================
{
    r.setbit(m_cmpx_con, CPOL, tf);
}

//=============================================================================
    bool        Comp::evt_bit           ()
//=============================================================================
{
    return r.anybit(m_cmpx_con, CEVT);
}

//=============================================================================
    bool        Comp::out_bit           ()
//=============================================================================
{
    return r.anybit(m_cmpx_con, COUT);
}

//=============================================================================
    void        Comp::evt_sel           (EVPOL e)
//=============================================================================
{
    r.clrbit(m_cmpx_con, EVPOL_CLR<<EVPOL_SHIFT);
    r.setbit(m_cmpx_con, e<<EVPOL_SHIFT);
}

//=============================================================================
    void        Comp::cref_cxina        (bool tf)
//=============================================================================
{
    r.setbit(m_cmpx_con, CREF, !tf);
}

//=============================================================================
    void        Comp::ch_sel            (CCH e)
//=============================================================================
{
    r.clrbit(m_cmpx_con, BGAP);
    r.setbit(m_cmpx_con, e);
}

//common static functions
//=============================================================================
    void        Comp::stop_idle         (bool tf)
//=============================================================================
{
    r.setbit(CMSTAT, SIDL, tf);
}

//=============================================================================
    void        Comp::cref_sel          (CVREF e)
//=============================================================================
{
    r.setbit(CMSTAT, CVREFSEL, e);
}