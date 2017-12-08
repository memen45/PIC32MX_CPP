#pragma once

/*=============================================================================
 Timer1 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

namespace Timer1 {

    enum
    {
        T1CON = 0xBF808000,
        TMR1 =  0xBF808010,
        PR1 =   0xBF808020,
        ON = 1<<15,     SIDL = 1<<13,    TWDIS = 1<<12,   TWIP = 1<<11,
        EXT_RES = 3<<8, EXT_LPRC = 2<<8, EXT_T1CK = 1<<8, EXT_SOSC = 0<<8,
        TGATE = 1<<7,
        TCKPS_256 = 3<<4, TCKPS_64 = 2<<4, TCKPS_8 = 1<<4, TCKPS_1 = 0<<4,
        TSYNC = 1<<2,
        TCS = 1<<1
    };

    void timer( uint16_t n ){       Reg::val( TMR1, n ); }
    uint16_t timer( void ){         return Reg::val( TMR1 ); }

    void pr( uint16_t n ){          Reg::val( PR1, n ); }
    uint16_t pr( void ){            return Reg::val( PR1 ); }

    void on( bool tf ){             Reg::set( T1CON, ON, tf ); }
    void stop_idle( bool tf ){      Reg::set( T1CON, SIDL, tf ); }
    void async_wrdis( bool tf ){    Reg::set( T1CON, TWDIS, tf ); }
    bool wr_busy( void ){           return Reg::is_set( T1CON, TWIP ); }

    void clk_sosc( void ){          Reg::clr( T1CON, EXT_RES ); }
    void clk_lprc( void ){          clk_sosc(); Reg::set( T1CON, EXT_LPRC ); }
    void clk_t1ck( void ){          clk_sosc(); Reg::set( T1CON, EXT_T1CK) ; }

    void tgate_on( bool tf ){       Reg::set( T1CON, TGATE, tf ); }

    void pre_1( void ){             Reg::clr( T1CON, TCKPS_256 ); }
    void pre_256( void ){           Reg::set( T1CON, TCKPS_256 ); }
    void pre_64( void ){            pre_1(); Reg::set( T1CON, TCKPS_64 ); }
    void pre_8( void ){             pre_1(); Reg::set( T1CON, TCKPS_8 ); }

    void tsync_on( bool tf ){       Reg::set( T1CON, TSYNC, tf ); }
    void ext_clk( bool tf ){        Reg::set( T1CON, TCS, tf ); }


};

