#pragma once

#include "Uart.hpp"

struct Putc {

    //set which uart device uses _mon_putc()
    static void use(Uart*);

    //enable/disable simple ansi markdown
    static void ansi(bool);

};
