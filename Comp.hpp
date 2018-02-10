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

    /*constexpr*/ Comp(CMX);

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
