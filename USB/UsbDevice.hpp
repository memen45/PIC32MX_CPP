#pragma once

#include <cstdint>
#include "Uart.hpp"


struct UsbDevice {

    static uint32_t timer1ms;
    static uint32_t sof_count;

    //init- true=detach/then attach, false=detach
    //return true if attached, false if not
    static bool         init        (bool);

    static bool         cdc_tx      (uint8_t*, uint16_t);
    static bool         cdc_rx      (uint8_t*, uint16_t);

};
