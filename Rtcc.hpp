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
        RTCCON1 = 0xBF800000,
        RTCCON2 = 0xBF800010,
        RTCSTAT = 0xBF800030,
        RTCTIME = 0xBF800040,
        RTCDATE = 0xBF800050,
        ALMTIME = 0xBF800060,
        ALMDATE = 0xBF800070,
        ALARM = 1<<31, CHIME = 1<<30, ALMRPTCLR = 7<<16,
        ON = 1<<15, WRLOCK = 1<<11, PINON = 1<<7,
        ALMSTAT = 1<<5, SYSNCSTAT = 1<<2,
        ALMSYNCSTAT = 1<<1, HALFSTAT = 1<<0
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

    enum PRESCALE
    {
        PRE256 = 3<<4, PRE64 = 2<<4, PRE16 = 1<<4, PRE1 = 0<<4
    };

    enum CLKSEL
    {
        FCY = 3, PWRLPIN = 2, LPRC = 1, SOSC = 0
    };

    //RTCCON1 lock off by default, these functions will lock RTCCON1 when done
    //private (not really)
    void _unlock( void ){   Syskey::unlock(); Reg::clr( RTCCON1, WRLOCK ); }
    void _lock( void ){     Reg::set( RTCCON1, WRLOCK ); Syskey::lock(); }
    void _conset( uint32_t r, uint32_t v )
    {
        _unlock(); Reg::set( r, v ); _lock();
    }
    void _conclr( uint32_t r, uint32_t v )
    {
        _unlock(); Reg::clr( r, v ); _lock();
    }
    void _conval( uint32_t r, uint32_t v )
    {
        _unlock(); Reg::val( r, v ); _lock();
    }
    //--private
    void alarm_on( void ){              _conset( RTCCON1, ALARM ); }
    void alarm_off( void ){             _conclr( RTCCON1, ALARM ); }
    void chime_on( void ){              _conset( RTCCON1, CHIME ); }
    void chime_off( void ){             _conclr( RTCCON1, CHIME ); }
    void alarm_interval( AMASK v )
    {
        _conclr( RTCCON1, AMASKCLR );
        _conset( RTCCON1, v );
    }
    void alarm_repeat( uint8_t v ){
        _conclr( RTCCON1, ALMRPTCLR );
        _conset( RTCCON1, v );
    }
    void on( void ){                    _conset( RTCCON1, ON ); }
    void off( void ){                   _conclr( RTCCON1, ON ); }
    void pin_on( void ){                _conset( RTCCON1, PINON ); }
    void pin_off( void ){               _conclr( RTCCON1, PINON ); }
    void pin_src( OUTSEL v ){
        _conclr( RTCCON1, CLRSEL );
        _conclr( RTCCON1, v );
    }

    void clk_div( uint16_t v ){
        _unlock();
        Reg::val16( RTCCON2+2, v );
        _lock();
    }
    void clk_frdiv( uint8_t v )
    {
        _conclr( RTCCON2, 31<<11 );
        _conset( RTCCON2, (v&31)<<11 );
    }
    void prescale( PRESCALE p )
    {
        _conclr( RTCCON2, PRE256 );
        _conset( RTCCON2, p );
    }
    void clk_sel( CLKSEL c )
    {
        _conclr( RTCCON2, FCY );
        _conset( RTCCON2, c );
    }

    bool alarm_evt( void ){         return Reg::is_set( RTCSTAT, ALMSTAT ); }
    bool time_busy( void ){         return Reg::is_set( RTCSTAT, SYSNCSTAT ); }
    bool alarm_busy( void ){        return Reg::is_set( RTCSTAT, ALMSYNCSTAT );}
    bool half_sec( void ){          return Reg::is_set( RTCSTAT, HALFSTAT ); }

    //raw time, date
    uint32_t time( void ){          return Reg::val( RTCTIME ); }
    uint32_t time_alarm( void ){    return Reg::val( ALMTIME ); }
    uint32_t time_sec( void )
    {
        uint32_t t = time()>>8;
        uint32_t s = 0;
        uint16_t m[] = { 1, 10, 60, 600, 3600, 36000 };
        for( auto i = 0; i < 6; i++, t>>=4 ){ s += (t&15)*m[i]; }
        return s;
    }
    void time( uint32_t v ){        _conval( RTCTIME, v ); }
    void time_alarm( uint32_t v ){  Reg::val( ALMTIME, v ); }
    uint32_t date( void ){          return Reg::val( RTCDATE ); }
    uint32_t date_alarm( void ){    return Reg::val( ALMDATE ); }
    void date( uint32_t v ){        _conval( RTCTIME, v ); }
    void date_alarm( uint32_t v ){  Reg::val( ALMTIME, v ); }

};

