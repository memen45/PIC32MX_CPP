#pragma once

#include <cstdint>
#include "UsbDescriptors.hpp"
#include "UsbEP.hpp"
#include "UsbDevice.hpp"


struct UsbCdcAcm {

    static bool init(bool);

    static bool service(uint32_t, uint8_t);

};
