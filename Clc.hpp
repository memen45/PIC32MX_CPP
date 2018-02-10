#pragma once

//CLC1-4

#include <cstdint>
#include "Reg.hpp"

struct Clc {

    //instantiate Clc with clc number
    enum CLCX { CLC1 = 0, CLC2, CLC3, CLC4 };

    //logic function
    enum MODE : uint8_t { ANDOR = 0, ORXOR, AND, SR, DSR, DR, JKR, LSR };

    //gate polarity
    enum GXPOL { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };

    /*constexpr*/ Clc(CLCX);

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
