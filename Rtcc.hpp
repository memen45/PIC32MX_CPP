#pragma once

/*=============================================================================
 Real-Time Clock & Calendar (RTCC)

 not sure exactly when wrlock is required- various info from datasheet and
 reference manual are not really that specific, or info conflicts
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
        ALARMEN = 1<<31, CHIME = 1<<30, ALMRPTCLR = 7<<16,
        ON = 1<<15, WRLOCK = 1<<11, PINON = 1<<7,
        ALMSTAT = 1<<5, SYSNCSTAT = 1<<2,
        ALMSYNCSTAT = 1<<1, HALFSTAT = 1<<0,
        CLK_DIV_32KHZ = 0x3FFF
    };

    enum AMASK
    {
        YEAR = 9<<24, MONTH = 8<<24, WEEK = 7<<24, DAY = 6<<24, HOUR= 5<<24,
        MINUTE10 = 4<<24, MINUTE1 = 3<<24, SECOND10 = 2<<24, SECOND1 = 1<<24,
        HALFSEC = 0,
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
    //private (not really, this is a namespace so just prepend _ to make it
    //look private- don't use elsewhere stuff starting with a _ )
    void _unlock( void ){   Syskey::unlock(); Reg::clr( RTCCON1, WRLOCK ); }
    void _lock( void ){     Reg::set( RTCCON1, WRLOCK ); Syskey::lock(); }
    void _conset( uint32_t r, uint32_t v, bool tf )
    {
        _unlock(); Reg::set( r, v, tf ); _lock();
    }
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

    void alarm( bool tf ){              _conset( RTCCON1, ALARMEN, tf ); }
    void chime( bool tf ){              _conset( RTCCON1, CHIME, tf ); }
    void alarm_interval( AMASK e )
    {
        _conclr( RTCCON1, AMASKCLR );
        _conset( RTCCON1, e );
    }
    void alarm_repeat( uint8_t v ){
        _conclr( RTCCON1, ALMRPTCLR );
        _conset( RTCCON1, v );
    }
    void on( bool tf ){                 _conset( RTCCON1, ON, tf ); }
    void out( bool tf ){                _conset( RTCCON1, PINON, tf ); }
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
    void clk_pre( PRESCALE e )
    {
        _conclr( RTCCON2, PRE256 );
        _conset( RTCCON2, e );
    }
    void clk_sel( CLKSEL e )
    {
        _conclr( RTCCON2, FCY );
        _conset( RTCCON2, e );
    }

    bool alarm_evt( void ){         return Reg::is_set( RTCSTAT, ALMSTAT ); }
    bool time_busy( void ){         return Reg::is_set( RTCSTAT, SYSNCSTAT ); }
    bool alarm_busy( void ){        return Reg::is_set( RTCSTAT, ALMSYNCSTAT );}
    bool half_sec( void ){          return Reg::is_set( RTCSTAT, HALFSTAT ); }

    //raw time, date
    uint32_t time( void ){          return Reg::val( RTCTIME ); }
    uint32_t date( void ){          return Reg::val( RTCDATE ); }
    uint32_t alarm_time( void ){    return Reg::val( ALMTIME ); }
    uint32_t alarm_date( void ){    return Reg::val( ALMDATE ); }
    void time( uint32_t v ){        _conval( RTCTIME, v ); } //wrlock
    void date( uint32_t v ){        _conval( RTCTIME, v ); } //wrlock
    void alarm_time( uint32_t v ){  Reg::val( ALMTIME, v ); }
    void alarm_date( uint32_t v ){  Reg::val( ALMTIME, v ); }
};

