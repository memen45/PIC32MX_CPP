#pragma once

#include <cstdint>
#include "Reg.hpp"

/*=============================================================================
 Compare1/2/3 functions
=============================================================================*/

class Comp123  {

    public:

    //instantiate Comp123 with comparator number
    enum CMX { C1 = 0, C2, C3 };
    //event polarity
    enum EVPOL { ANY = 3<<6, HL = 2<<6, LH = 1<<6, OFF = 0 };
    //channel select
    enum CCH { BGAP = 3, CXIND = 2, CXINC = 1, CXINB = 0 };

    //public functions
    void            on          (bool);
    void            out         (bool);
    void            out_inv     (bool);
    bool            event_bit   (void);
    bool            out_bit     (void);
    void            event_sel   (EVPOL);
    void            cref_refsel (void);
    void            cref_cxina  (void);
    void            ch_sel      (CCH);
    //common for all instances
    static void     stop_idle   (bool);
    static void     ref_ext     (bool);

    //constructor
    constexpr Comp123(CMX);

    private:

    enum {
        ON = 1<<15, COE = 1<<14, CPOL = 1<<13,
        CEVT = 1<<9, COUT = 1<<8, CREF = 1<<4,
        CMSTAT = 0xBF802300,
        CM1CON = 0xBF802310, CMXCON_SPACING = 8, //spacing in words
        SIDL = 1<<13, CVREFSEL = 1<<8
    };

    //private vars
    volatile uint32_t * m_pt;
};


/*=============================================================================
 all functions inline
=============================================================================*/

constexpr Comp123::Comp123(CMX e)
    : m_pt((volatile uint32_t*)CM1CON+(e*CMXCON_SPACING)){}

void Comp123::on(bool tf){ Reg::set(m_pt, ON, tf); }
void Comp123::out(bool tf){ Reg::set(m_pt, COE, tf); }
void Comp123::out_inv(bool tf){ Reg::set(m_pt, CPOL, tf); }
bool Comp123::event_bit(void){ return Reg::is_set(m_pt, CEVT); }
bool Comp123::out_bit(void){ return Reg::is_set(m_pt, COUT); }
void Comp123::event_sel(EVPOL e){ Reg::clr(m_pt, ANY); Reg::set(m_pt, e); }
void Comp123::cref_refsel(void){ Reg::set(m_pt, CREF); }
void Comp123::cref_cxina(void){ Reg::clr(m_pt, CREF); }
void Comp123::ch_sel(CCH e){ Reg::clr(m_pt, BGAP); Reg::set(m_pt, e); }

//common static functions
void Comp123::stop_idle(bool tf){ Reg::set(CMSTAT, SIDL, tf); }
void Comp123::ref_ext(bool tf){ Reg::set(CMSTAT, CVREFSEL, tf); }