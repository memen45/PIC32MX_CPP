#pragma once

#include <cstdint>
#include "Uart.hpp"

struct UsbDevice {

    //really no need to keep track of usb states
    //all info is available elsewhere, such as-
    //if(not Usb::power(Usb::USBPWR)){ /*DETACHED*/ }
    //else if(not Usb::dev_addr()){ /**/ }
    //if(Usb::dev_addr()){ /*>ADDRESS*/ }
    //else { /*<ADDRESS*/ }
    //if(Usb::irq(Usb::RESUME)){ /*SUSPENDED*/ }
    //etc.


    static uint32_t timer1ms;
    static uint32_t sof_count;

    //init- true=detach/then attach, false=detach
    //return true if attached, false if not
    static bool         init        (bool);

    static bool         cdc_tx      (uint8_t*, uint16_t);
    static bool         cdc_rx      (uint8_t*, uint16_t);

};
