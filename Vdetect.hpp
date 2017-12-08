#pragma once

/*=============================================================================
 High/Low-Voltage Detect functions
=============================================================================*/

namespace Vdetect {

    enum
    {
        HLVDCON =    0xBF802920,
        HLVDCONCLR = 0xBF802924,
        HLVDCONSET = 0xBF802928,
        ON = 1<<15, SIDL = 1<<13, VDIR = 1<<11,
        BGVST = 1<<10, IRVST = 1<<9,
        HLEVT = 1<<8
    };

    enum HLVDL
    {
        HLVDL_EXT = 15, HLVDL_200_222 = 14, HLVDL_208_233 = 13,
        HLVDL_215_244 = 12, HLVDL_225_255 = 11, HLVDL_235_269 = 10,
        HLVDL_245_280 = 9, HLVDL_265_298 = 8, HLVDL_275_309 = 7,
        HLVDL_295_330 = 6, HLVDL_325_363 = 5
    };

    void on( bool tf ){             Reg::set( HLVDCON, ON, tf ); }
    void stop_idle( bool tf ){      Reg::set( HLVDCON, SIDL, tf ); }
    void trip_above( bool tf ){     Reg::set( HLVDCON, VDIR, tf ); }
    bool bgap_stable( void ){       return Reg::is_set( HLVDCON, BGVST ); }
    bool iref_stable( void ){       return Reg::is_set( HLVDCON, IRVST ); }
    bool tripped( void ){           return Reg::is_set( HLVDCON, HLEVT ); }

    void limit_set( HLVDL lvl )
    {
        Reg::clr( HLVDCON, HLVDL_EXT );
        Reg::set( HLVDCON, lvl );
    }
};

