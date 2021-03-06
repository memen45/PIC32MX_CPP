#include "Vdetect.hpp"

enum {
HLVDCON = 0xBF802920,
    ON = 15,
    SIDL = 13,
    VDIR = 11,
    BGVST = 10,
    IRVST = 9,
    HLEVT = 8
};

//=============================================================================
            auto Vdetect::
on          (bool tf) -> void
            {
            setbit(HLVDCON, 1<<ON, tf);
            }

//=============================================================================
            auto Vdetect::
stop_idle   (bool tf) -> void
            {
            setbit(HLVDCON, 1<<SIDL, tf);
            }

//=============================================================================
            auto Vdetect::
trip_above  (bool tf) -> void
            {
            setbit(HLVDCON, 1<<VDIR, tf);
            }

//=============================================================================
            auto Vdetect::
bgap_stable () -> bool
            {
            return anybit(HLVDCON, 1<<BGVST);
            }

//=============================================================================
            auto Vdetect::
iref_stable () -> bool
            {
            return anybit(HLVDCON, 1<<IRVST);
            }

//=============================================================================
            auto Vdetect::
tripped     () -> bool
            {
            return anybit(HLVDCON, 1<<HLEVT);
            }

//=============================================================================
            auto Vdetect::
limit       (HLVDL e) -> void
            {
            clrbit(HLVDCON, VEXT);
            setbit(HLVDCON, e);
            }
