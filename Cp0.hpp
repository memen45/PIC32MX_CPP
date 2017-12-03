#ifndef _CP0_H
#define _CP0_H

#include <cstdint>

namespace Cp0 {

    uint32_t count( void ){         return __builtin_mfc0(9,0); }
    void count( uint32_t v ){       __builtin_mtc0(9,0,v); }
    uint32_t compare( void ){       return __builtin_mfc0(11,0); }
    void compare( uint32_t v ){     __builtin_mtc0(11,0,v); }
}

#endif //CP0_H
