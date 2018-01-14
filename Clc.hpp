#pragma once

/*=============================================================================
 CLC1-4 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Clc {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate Clc with clc number
    enum CLC { CLC1 = 0, CLC2, CLC3, CLC4 };

    //logic function
    enum MODE : uint8_t { ANDOR = 0, ORXOR, AND, SR, DSR, DR, JKR, LSR };

    //gate polarity
    enum GXPOL { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    constexpr Clc(CLC);

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

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    static Reg r;

    enum {
        CLC1CON = 0xBF802480, CLCXCON_SPACING = 32, //spacing in words
            ON = 1<<15,
            SIDL = 1<<13,
            INTP = 1<<11,
            INTN = 1<<10,
            LCOE = 1<<7,
            LCOUT = 1<<6,
            LCPOL = 1<<5,
        CLCXSEL = 4, //offsets in words
        CLCXGLS = 8, //offsets in words
    };

    //private vars
    volatile uint32_t* m_clcx_con;

};


/*=============================================================================
 all functions inline
=============================================================================*/

constexpr Clc::Clc(CLC e)
    : m_clcx_con((volatile uint32_t*)CLC1CON+(e*CLCXCON_SPACING))
{}

void Clc::gate_inv(GXPOL e, bool tf){
    r.setbit(m_clcx_con, e, tf);
}
void Clc::on(bool tf){
    r.setbit(m_clcx_con, ON, tf);
}
void Clc::stop_idle(bool tf){
    r.setbit(m_clcx_con, SIDL, tf);
}
void Clc::intp(bool tf){
    r.setbit(m_clcx_con, INTP, tf);
}
void Clc::intn(bool tf){
    r.setbit(m_clcx_con, INTN, tf);
}
void Clc::out(bool tf){
    r.setbit(m_clcx_con, LCOE, tf);
}
bool Clc::out(){
    return r.anybit(m_clcx_con, LCOUT);
}
void Clc::out_inv(bool tf){
    r.setbit(m_clcx_con, LCPOL, tf);
}
void Clc::mode(MODE e){
    r.clrbit(m_clcx_con, LSR);
    r.setbit(m_clcx_con, e);
}
//input select, n = 1-4, v = 0-7 (invalid args masked to good vals)
void Clc::in_sel(uint8_t n, uint8_t v){
    n -= 1; n &= 3; n <<= 2; v &= 7;
    r.clrbit(m_clcx_con+CLCXSEL, 7<<n);
    r.setbit(m_clcx_con+CLCXSEL, v<<n);
}
//or all in in shot with precomputed value
void Clc::in_sel(uint32_t v){
    r.val(m_clcx_con+CLCXSEL, v);
}
//gate select, n = 1-4 (invalid gate masked to good gate)
void Clc::gate_sel(uint8_t n, uint8_t v){
    n -= 1; n &= 3; n <<= 3;
    r.clrbit(m_clcx_con+CLCXGLS, 15<<n);
    r.setbit(m_clcx_con+CLCXGLS, v<<n);
}
//or all in in shot with precomputed value
void Clc::gate_sel(uint32_t v){
    r.val(m_clcx_con+CLCXGLS, v);
}
