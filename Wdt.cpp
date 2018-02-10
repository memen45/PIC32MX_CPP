#include "Wdt.hpp"

//16bit write to upper 16bits of WDTCON
//=============================================================================
    void        Wdt::reset              ()
//=============================================================================
{
    r.val(WDTCON + 2, (uint16_t)CLRKEY);
}

//=============================================================================
    void        Wdt::on                 (bool tf)
//=============================================================================
{
    r.setbit(WDTCON, ON, tf);
}

//=============================================================================
    void        Wdt::window_on          (bool tf)
//=============================================================================
{
    r.setbit(WDTCON, WINEN, tf);
}
