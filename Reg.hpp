#ifndef _REG_H
#define _REG_H

#include <cstdint>

namespace Reg {

    enum {
        CLR = 1, SET = 2, INV = 3,
        SYSKEY_ADDR = 0xBF803670
    };
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

    void syskey_lock( void ){       val( SYSKEY_ADDR, 0 ); }
    bool syskey_unlock( void )
    {
        syskey_lock();
        val( SYSKEY_ADDR, 0xAA996655 );
        val( SYSKEY_ADDR, 0x556699AA );
        return val( SYSKEY_ADDR ) != 0; //succeed = true
    }
}

#endif //_REG_H
