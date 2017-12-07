#pragma once

/*=============================================================================
 Register (SFR) writes/read, syskey lock/unlock (in here for now)
=============================================================================*/

#include <cstdint>

namespace Reg {

    enum { CLR = 1, SET = 2, INV = 3 };

    //all register values cast to volatile uint32_t*
    //so can use enum values as register argument
    //SET/CLR/INV offsets will be calculated in words (4 bytes)
    template <typename T>
    void set( T r, uint32_t v )
    {
        *(reinterpret_cast <volatile uint32_t*>(r)+SET) = v;
    }

    template <typename T>
    void clr( T r, uint32_t v )
    {
        *(reinterpret_cast <volatile uint32_t*>(r)+CLR) = v;
    }

    template <typename T>
    void inv( T r, uint32_t v )
    {
        *(reinterpret_cast <volatile uint32_t*>(r)+INV) = v;
    }

    template <typename T>
    bool is_set( T r, uint32_t v )
    {
        return *(reinterpret_cast <volatile uint32_t*>(r)) & v;
    }

    template <typename T>
    bool is_clr( T r, uint32_t v )
    {
        return ! *(reinterpret_cast <volatile uint32_t*>(r)) & v;
    }

    template <typename T>
    uint32_t val( T r )
    {
        return *(reinterpret_cast <volatile uint32_t*>(r));
    }

    template <typename T>
    void val( T r, uint32_t v )
    {
        *(reinterpret_cast <volatile uint32_t*>(r)) = v;
    }

    //for wdt reset - needs to do a 16bit write
    template <typename T>
    void val16( T r, uint16_t v )
    {
        *(reinterpret_cast <volatile uint16_t*>(r)) = v;
    }
};

