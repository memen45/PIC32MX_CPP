#pragma once

#include <cstdint>
#include "Reg.hpp"

/*=============================================================================
 Compare 1/2/3 functions
=============================================================================*/

struct Comp123  {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate Comp123 with comparator number
    enum CMX { CMP1 = 0, CMP2, CMP3 };

    //event polarity
    enum EVPOL { OFF = 0, LH, HL, ANY };

    //channel select
    enum CCH { CXINB = 0, CXINC, CXIND, BGAP };

    //cref source
    enum CVREF { INT_BGAP = 0, EXT_CVREF };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    constexpr Comp123(CMX);

    void            on              (bool);
    void            out             (bool);
    void            out_inv         (bool);
    bool            evt_bit         ();
    bool            out_bit         (void);
    void            evt_sel         (EVPOL);
    void            cref_cxina      (bool);
    void            ch_sel          (CCH);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //common for all instances

    static void     stop_idle       (bool);
    static void     cref_sel        (CVREF);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
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

    volatile uint32_t* m_cmxcon;
};


/*=============================================================================
 all functions inline
=============================================================================*/

constexpr Comp123::Comp123(CMX e)
    : m_cmxcon((volatile uint32_t*)CM1CON+(e*CMXCON_SPACING)){}

void Comp123::on(bool tf){
    r.setbit(m_cmxcon, ON, tf);
}
void Comp123::out(bool tf){
    r.setbit(m_cmxcon, COE, tf);
}
void Comp123::out_inv(bool tf){
    r.setbit(m_cmxcon, CPOL, tf);
}
bool Comp123::evt_bit(){
    return r.anybit(m_cmxcon, CEVT);
}
bool Comp123::out_bit(){
    return r.anybit(m_cmxcon, COUT);
}
void Comp123::evt_sel(EVPOL e){
    r.clrbit(m_cmxcon, EVPOL_CLR<<EVPOL_SHIFT);
    r.setbit(m_cmxcon, e<<EVPOL_SHIFT);
}
void Comp123::cref_cxina(bool tf){
    r.setbit(m_cmxcon, CREF, !tf);
}
void Comp123::ch_sel(CCH e){
    r.clrbit(m_cmxcon, BGAP);
    r.setbit(m_cmxcon, e);
}

//common static functions
void Comp123::stop_idle(bool tf){
    r.setbit(CMSTAT, SIDL, tf);
}
void Comp123::cref_sel(CVREF e){
    r.setbit(CMSTAT, CVREFSEL, e);
}