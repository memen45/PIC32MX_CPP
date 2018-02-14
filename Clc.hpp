#pragma once

//CLC1-4

#include <cstdint>

struct Clc {

    //instantiate Clc with CLCn
    enum CLCX { CLC1 = 0, CLC2, CLC3, CLC4 };
    Clc(CLCX);

    enum GXPOL { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };
    void    gate_inv    (GXPOL, bool);

    void    on          (bool);
    void    stop_idle   (bool);
    void    intp        (bool);
    void    intn        (bool);
    void    out         (bool);
    bool    out         ();
    void    out_inv     (bool);

    enum MODE : uint8_t { ANDOR = 0, ORXOR, AND, SR, DSR, DR, JKR, LSR };
    void    mode        (MODE e);

    void    in_sel      (uint8_t, uint8_t);
    void    in_sel      (uint32_t);
    void    gate_sel    (uint8_t, uint8_t);
    void    gate_sel    (uint32_t);

    private:

    //private vars
    volatile uint32_t* m_clcx_con;

};
