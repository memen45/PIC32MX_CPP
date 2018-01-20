#pragma once

//Watchdog timer

#include <cstdint>
#include "Reg.hpp"

struct Wdt {

    static void     reset       ();
    static void     on          (bool);
    static void     window_on   (bool);

    private:

    static Reg r;

    enum {
        WDTCON = 0xBF803990,
            CLRKEY = 0x5743,
            ON = 1<<15,
            WINEN = 1<<0
        //RUNDIV, SLPDIV, CLKSEL are read-only and are set in config bits
        //if need to read them, can make your own read function(s) here or use
        //uint32_t val = Reg::val(Wdt::WDTCON); -then decipher on your own
    };

};

//16bit write to upper 16bits of WDTCON
//=============================================================================
    void        Wdt::reset              ()
//=============================================================================
{
    r.val(WDTCON+2, (uint16_t)CLRKEY);
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