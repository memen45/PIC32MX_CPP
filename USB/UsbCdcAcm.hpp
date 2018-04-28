#pragma once

#include "UsbEP.hpp"
#include <cstdint>



struct UsbCdcAcm {

    static bool init(bool);

    static bool send(uint8_t*, uint16_t);

};
