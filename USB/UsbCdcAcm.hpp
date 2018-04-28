#pragma once

#include "UsbEP.hpp"
#include <cstdint>



struct UsbCdcAcm {

    static bool init(bool);

    static bool service(uint8_t, UsbEP0* = 0);

    static bool send(uint8_t*, uint16_t);

};
