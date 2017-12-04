#ifndef _REG_H
#define _REG_H

#include <cstdint>

namespace Reg {

    enum {
        CLR = 1, SET = 2, INV = 3
    };
    //all register values r cast to volatile uint32_t*
    //so can use enum values as register
//    template <typename _t>
//    void set( _t r, uint32_t v ){       *(volatile uint32_t*)(r+SET) = v; }
//    template <typename _t>
//    void clr( _t r, uint32_t v ){       *(volatile uint32_t*)(r+CLR) = v; }
//    template <typename _t>
//    void inv( _t r, uint32_t v ){       *(volatile uint32_t*)(r+INV) = v; }
//    template <typename _t>
//    bool is_set( _t r, uint32_t v ){    return *(volatile uint32_t*)r & v; }
//    template <typename _t>
//    bool is_clr( _t r, uint32_t v ){    return ! *(volatile uint32_t*)r & v; }
//    template <typename _t>
//    uint32_t val( _t r ){               return *(volatile uint32_t*)r; }
//    template <typename _t>
//    void val( _t r, uint32_t v ){       *(volatile uint32_t*)r = v; }

    void set( volatile uint32_t* r, uint32_t v ){       *(volatile uint32_t*)(r+SET) = v; }

    void clr( volatile uint32_t* r, uint32_t v ){       *(volatile uint32_t*)(r+CLR) = v; }

    void inv( volatile uint32_t* r, uint32_t v ){       *(volatile uint32_t*)(r+INV) = v; }

    bool is_set( volatile uint32_t* r, uint32_t v ){    return *(volatile uint32_t*)r & v; }

    bool is_clr( volatile uint32_t* r, uint32_t v ){    return ! *(volatile uint32_t*)r & v; }

    uint32_t val( volatile uint32_t* r ){               return *(volatile uint32_t*)r; }

    void val( volatile uint32_t* r, uint32_t v ){       *(volatile uint32_t*)r = v; }
}

#endif //_REG_H
