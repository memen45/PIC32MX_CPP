#pragma once

#include "UsbEP.hpp"
#include <cstdint>

struct UsbCdcAcm {

    static auto
    init (bool) -> bool;

    static auto
    send (uint8_t*, uint16_t, UsbEP::notify_t = 0) -> bool;

    static auto
    recv (uint8_t*, uint16_t, UsbEP::notify_t = 0) -> bool;

    static auto
    is_active () -> bool;

};
