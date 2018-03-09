//USB Config settings

#include <cstdint>
#include "UsbConfig.hpp"

Pins UsbConfig::vbus_pin(UsbConfig::vbus_pin_n, Pins::IN);   //Vbus pin

UsbEndpt UsbConfig::endpoints[5] = {
    {0, UsbEndpt::TRX,  64}, //endpoint 0
    {1, UsbEndpt::TX,   64},
    {2, UsbEndpt::RX,   64},
    {3, UsbEndpt::NONE, 0},
    {4, UsbEndpt::RX,   64}
};



