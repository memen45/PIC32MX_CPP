#ifndef _REG_H
#define _REG_H

#include <cstdint>

namespace Reg {

    enum {
        CLR = 1, SET = 2, INV = 3
    };

    void set( volatile uint32_t* r, uint32_t v ){       *(r+SET) = v; }
    void clr( volatile uint32_t* r, uint32_t v ){       *(r+CLR) = v; }
    void inv( volatile uint32_t* r, uint32_t v ){       *(r+INV) = v; }
    bool is_set( volatile uint32_t* r, uint32_t v ){    return *r & v; }
    bool is_clr( volatile uint32_t* r, uint32_t v ){    return ! *r & v; }
    uint32_t val( volatile uint32_t* r ){               return *r; }
    void val( volatile uint32_t* r, uint32_t v ){       *r = v; }
}

#endif //_REG_H
