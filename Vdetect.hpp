#pragma once

//High/Low-Voltage Detect

#include <cstdint>
#include "Reg.hpp"

struct Vdetect {

    //voltage level limits
    enum HLVDL {
        VEXT = 15, V200_222 = 14, V208_233 = 13, V215_244 = 12, V225_255 = 11,
        V235_269 = 10, V245_280 = 9, V265_298 = 8, V275_309 = 7, V295_330 = 6,
        V325_363 = 5
    };

    static void     on              (bool);
    static void     stop_idle       (bool);
    static void     trip_above      (bool);
    static bool     bgap_stable     ();
    static bool     iref_stable     ();
    static bool     tripped         ();
    static void     limit           (HLVDL);

    private:

    static Reg r;

    enum {
        HLVDCON = 0xBF802920,
            ON = 1<<15,
            SIDL = 1<<13,
            VDIR = 1<<11,
            BGVST = 1<<10,
            IRVST = 1<<9,
            HLEVT = 1<<8
    };

};

//=============================================================================
    void        Vdetect::on             (bool tf)
//=============================================================================
{
    r.setbit(HLVDCON, ON, tf);
}

//=============================================================================
    void        Vdetect::stop_idle      (bool tf)
//=============================================================================
{
    r.setbit(HLVDCON, SIDL, tf);
}

//=============================================================================
    void        Vdetect::trip_above     (bool tf)
//=============================================================================
{
    r.setbit(HLVDCON, VDIR, tf);
}

//=============================================================================
    bool        Vdetect::bgap_stable    ()
//=============================================================================
{
    return r.anybit(HLVDCON, BGVST);
}

//=============================================================================
    bool        Vdetect::iref_stable    ()
//=============================================================================
{
    return r.anybit(HLVDCON, IRVST);
}

//=============================================================================
    bool        Vdetect::tripped        ()
//=============================================================================
{
    return r.anybit(HLVDCON, HLEVT);
}

//=============================================================================
    void        Vdetect::limit          (HLVDL e)
//=============================================================================
{
    r.clrbit(HLVDCON, VEXT);
    r.setbit(HLVDCON, e);
}