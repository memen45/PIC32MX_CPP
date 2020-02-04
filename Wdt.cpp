#include "Wdt.hpp"

enum {
WDTCON = 0xBF800000,
    ON = 15,
    WINEN = 1,
    CLR = 0
//WDTPS is read-only and set in config bits
//if need to read them, can make your own read function(s)
};

//=============================================================================
            auto Wdt::
reset       () -> void
            {
            setbit(WDTCON, 1 << CLR);
            }

//=============================================================================
            auto Wdt::
on          (bool tf) -> void
            {
            setbit(WDTCON, 1<<ON, tf);
            }

//=============================================================================
            auto Wdt::
window_on   (bool tf) -> void
            {
            setbit(WDTCON, 1<<WINEN, tf);
            }
