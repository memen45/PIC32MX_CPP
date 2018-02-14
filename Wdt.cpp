#include "Wdt.hpp"
#include "Reg.hpp"

enum {
    WDTCON = 0xBF803990,
        CLRKEY = 0x5743,
        ON = 15,
        WINEN = 0
    //RUNDIV, SLPDIV, CLKSEL are read-only and are set in config bits
    //if need to read them, can make your own read function(s)
};

//16bit write to upper 16bits of WDTCON
//=============================================================================
    void        Wdt::reset              ()
//=============================================================================
{
    Reg::val(WDTCON + 2, (uint16_t)CLRKEY);
}

//=============================================================================
    void        Wdt::on                 (bool tf)
//=============================================================================
{
    Reg::setbit(WDTCON, 1<<ON, tf);
}

//=============================================================================
    void        Wdt::window_on          (bool tf)
//=============================================================================
{
    Reg::setbit(WDTCON, 1<<WINEN, tf);
}

