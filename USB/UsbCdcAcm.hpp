#pragma once

#include "UsbEP.hpp"
#include <cstdint>



struct UsbCdcAcm {

    static bool init(bool);

    static bool send(uint8_t*, uint16_t, UsbEP::notify_t = 0);
    static bool recv(uint8_t*, uint16_t, UsbEP::notify_t = 0);
};
