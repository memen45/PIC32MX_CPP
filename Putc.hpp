#pragma once

#include "Uart.hpp"
#include "UsbCdcAcm.hpp"

struct Putc {

    //set a uart device to use _mon_putc()
    static void use(Uart*);
    //set usb cdc device to use _mon_putc()
    static void use(UsbCdcAcm*);

    protected:

    Putc(){}
};
