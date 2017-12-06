#ifndef _WDT_H
#define _WDT_H

#include <cstdint>
#include "Reg.hpp"

namespace Wdt {

    enum {
        BASE_ADDR = 0xBF803990,
        CLRKEY = 0x5743,
        ON = 1<<15,
        WINEN = 1<<0
        //RUNDIV, SLPDIV, CLKSEL are read-only and are set in config bits
        //if need to read them, can make your own read function(s) here or use
        //uint32_t val = Reg::val( Wdt::BASE_ADDR ); -then decipher on your own
    };

    //16bit write to upper 16bits of BASE_ADDR
    //add 2 bytes to base to get the correct address for the 16bit write
    void reset( void ){
        Reg::val16( BASE_ADDR+2, CLRKEY );
    }
    void on( void ){            Reg::set( BASE_ADDR, ON ); }
    void off( void ){           Reg::clr( BASE_ADDR, ON ); }
    void window_on( void ){     Reg::set( BASE_ADDR, WINEN ); }
    void window_off( void ){    Reg::clr( BASE_ADDR, WINEN ); }

}

#endif //WDT_H
