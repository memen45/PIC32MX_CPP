#pragma once

/*=============================================================================
 Watchdog timer functions
=============================================================================*/

namespace Wdt {

    enum
    {
        WDTCON = 0xBF803990,
        CLRKEY = 0x5743,
        ON = 1<<15,
        WINEN = 1<<0
        //RUNDIV, SLPDIV, CLKSEL are read-only and are set in config bits
        //if need to read them, can make your own read function(s) here or use
        //uint32_t val = Reg::val( Wdt::WDTCON ); -then decipher on your own
    };

    //16bit write to upper 16bits of WDTCON
    //add 2 bytes to base to get the correct address for the 16bit write
    void reset( void ){             Reg::val16( WDTCON+2, CLRKEY ); }
    void on( bool tf ){             Reg::set( WDTCON, ON, tf ); }
    void window_on( bool tf ){      Reg::set( WDTCON, WINEN, tf ); }
};

