#pragma once

#include "UsbEP.hpp"
#include <cstdint>



struct UsbCdcAcm {

    static bool init(bool);

    static bool service(uint8_t, UsbEP* = 0);

};
