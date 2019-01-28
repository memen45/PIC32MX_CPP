#pragma once

#include "Uart.hpp"

struct Putc {

    //set which uart device uses _mon_putc()
    static void set(Uart*);

};
