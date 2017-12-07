#pragma once

/*=============================================================================
 Real-Time Clock & Calendar (RTCC)
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"

namespace Rtcc {

    enum
    {
        BASE_ADDR = 0xBF800000,
        ALARM = 1<<31, CHIME = 1<<30, ALMRPTCLR = 7<<16,
        ON = 1<<15, WRLOCK = 1<<11, PINON = 1<<7
    };

    enum AMASK
    {
        YEAR = 9<<24, MONTH = 8<<24, WEEK = 7<<24, DAY = 6<<24, HOUR= 5<<24,
        MIN10 = 4<<24, MIN = 3<<24, SEC10 = 2<<24, SEC = 1<<24, HALFSEC = 0,
        AMASKCLR = 15<<24,
    };

    enum OUTSEL
    {
        CLKSRC = 2<<4, CLKSEC = 1<<4, ALMEVT = 0<<4, CLRSEL = 7<<4
    };

    //RTCCON1 lock off by default, these functions will lock RTCCON1 when done
    //private (not really)
    void _unlock( void ){   Syskey::unlock(); Reg::clr( BASE_ADDR, WRLOCK ); }
    void _lock( void ){     Reg::set( BASE_ADDR, WRLOCK ); Syskey::lock(); }
    //--private

    void _con1set( uint32_t v ){ _unlock(); Reg::set( BASE_ADDR, v ); _lock(); }
    void _con1clr( uint32_t v ){ _unlock(); Reg::clr( BASE_ADDR, v ); _lock(); }
    void alarm_on( void ){              _con1set( ALARM ); }
    void alarm_off( void ){             _con1clr( ALARM ); }
    void chime_on( void ){              _con1set( CHIME ); }
    void chime_off( void ){             _con1clr( CHIME ); }
    void alarm_interval( AMASK v ){     _con1clr( AMASKCLR ); _con1set( v ); }
    void alarm_repeat( uint8_t v ){     _con1clr( ALMRPTCLR ); _con1set( v ); }
    void on( void ){                    _con1set( ON ); }
    void off( void ){                   _con1clr( ON ); }
    void pin_on( void ){                _con1set( PINON ); }
    void pin_off( void ){               _con1clr( PINON ); }
    void pin_src( OUTSEL v ){           _con1clr( CLRSEL ); _con1clr( v ); }

    //no lock required on below

    

};

