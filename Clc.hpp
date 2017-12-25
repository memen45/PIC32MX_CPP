#pragma once

/*=============================================================================
 CLC1-4 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Clc {

    //instantiate Clc with clc number
    enum CLC { CLC1 = 0, CLC2, CLC3, CLC4 };
    //logic function
    enum MODE : uint8_t { ANDOR = 0, ORXOR, AND, SR, DSR, DR, JKR, LSR };
    //gate polarity
    enum GXPOL { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };

    //public functions
    void    gate_inv    (GXPOL, bool);
    void    on          (bool);
    void    stop_idle   (bool);
    void    intp        (bool);
    void    intn        (bool);
    void    out         (bool);
    bool    out         ();
    void    out_inv     (bool);
    void    mode        (MODE e);
    void    in_sel      (uint8_t, uint8_t);
    void    in_sel      (uint32_t);
    void    gate_sel    (uint8_t, uint8_t);
    void    gate_sel    (uint32_t);

    //constructor
    constexpr Clc(CLC);

    private:

    static Reg r;

    enum {
        CLC1CON = 0xBF802480, CLCXCON_SPACING = 32, //spacing in words
        SEL_OFFSET = 4, GLS_OFFSET = 8, //offsets in words
        ON = 1<<15, SIDL = 1<<13, INTP = 1<<11 , INTN = 1<<10,
        LCOE = 1<<7, LCOUT = 1<<6, LCPOL = 1<<5
    };

    //private vars
    volatile uint32_t* m_pt;
};


/*=============================================================================
 all functions inline
=============================================================================*/

constexpr Clc::Clc(CLC e)
    : m_pt((volatile uint32_t*)CLC1CON+(e*CLCXCON_SPACING)){}

void Clc::gate_inv(GXPOL e, bool tf){ r.set(m_pt, e, tf); }
void Clc::on(bool tf){ r.set(m_pt, ON, tf); }
void Clc::stop_idle(bool tf){ r.set(m_pt, SIDL, tf); }
void Clc::intp(bool tf){ r.set(m_pt, INTP, tf); }
void Clc::intn(bool tf){ r.set(m_pt, INTN, tf); }
void Clc::out(bool tf){ r.set(m_pt, LCOE, tf); }
bool Clc::out(){ return r.is_set(m_pt, LCOUT); }
void Clc::out_inv(bool tf){ r.set(m_pt, LCPOL, tf); }
void Clc::mode(MODE e){ r.clr(m_pt, LSR); r.set(m_pt, e); }
//input select, dsn = 1-4, val = 0-7 (invalid args masked to good vals)
void Clc::in_sel(uint8_t dsn, uint8_t val){
    dsn -= 1; dsn &= 3; dsn <<= 2; val &= 7;
    r.clr(m_pt+SEL_OFFSET, 7<<dsn);
    r.set(m_pt+SEL_OFFSET, val<<dsn);
}
//or all in in shot with precomputed value
void Clc::in_sel(uint32_t val){ r.val(m_pt+SEL_OFFSET, val); }
//gate select, gate = 1-4 (invalid gate masked to good gate)
void Clc::gate_sel(uint8_t gate, uint8_t val){
    gate -= 1; gate &= 3; gate <<= 3;
    r.clr(m_pt+GLS_OFFSET, 15<<gate);
    r.set(m_pt+GLS_OFFSET, val<<gate);
}
//or all in in shot with precomputed value
void Clc::gate_sel(uint32_t val){ r.val(m_pt+GLS_OFFSET, val); }
