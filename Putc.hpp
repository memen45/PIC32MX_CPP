#pragma once

#include "Uart.hpp"

struct Putc {

    protected:

    //set which uart device uses _mon_putc()
    static void use(Uart*);

    Putc(){}
};
