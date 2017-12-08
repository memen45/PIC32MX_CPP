#pragma once

/*=============================================================================
 Coprocessor0  Count, Compare
=============================================================================*/

#include <cstdint>

namespace Cp0 {

    static uint8_t sysfreq = 24;        //used to calculate compare values
    static uint32_t m_compare_count;    //save count for reloads

    uint32_t count( void ){             return __builtin_mfc0(9,0); }   //get
    uint32_t compare( void ){           return __builtin_mfc0(11,0); }  //get
    void count( uint32_t v ){           __builtin_mtc0(9,0,v); }        //set
    void compare( uint32_t v ){         __builtin_mtc0(11,0,v); }       //set

    void compare_reload( void ){        compare( count() + m_compare_count ); }
    void compare_us( uint32_t us )
    {
        m_compare_count = sysfreq / 2 * us;
        compare_reload();
    }
    void compare_ms( uint16_t ms ){     compare_us( ms * 1000 ); }

}

