#ifndef _CP0_H
#define _CP0_H

#include <cstdint>

namespace Cp0 {

    uint32_t get_count( void ){         return __builtin_mfc0(9,0); }
    void set_count( uint32_t v ){       __builtin_mtc0(9,0,v); }
    uint32_t get_compare( void ){       return __builtin_mfc0(11,0); }
    void set_compare( uint32_t v ){     __builtin_mtc0(11,0,v); }
}

#endif //CP0_H
