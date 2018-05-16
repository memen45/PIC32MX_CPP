#pragma once

#include <cstdint>

//Compare 1/2/3

struct Comp {

        enum
    CMX { CMP1, CMP2, CMP3 };

    Comp(CMX);      //instantiate Comp with comparator number

        auto
    on (bool) -> void;

        auto
    out (bool) -> void;

        auto
    out_inv (bool) -> void;

        auto
    evt_bit () -> bool;

        auto
    out_bit (void) -> bool;

        enum
    EVPOL { OFF, LH, HL, ANY };

        auto
    evt_sel (EVPOL) -> void;

        auto
    cref_cxina (bool) -> void;

        enum
    CCH { CXINB, CXINC, CXIND, BGAP };

        auto
    ch_sel (CCH) -> void;

    //common for all instances
        static auto
    stop_idle (bool) -> void;

        enum
    CVREF { INT_BGAP, EXT_CVREF };

        static auto
    cref_sel (CVREF) -> void;


    private:

    volatile uint32_t* m_cmpx_con;

};
