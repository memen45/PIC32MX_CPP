#pragma once

#include "Irq.hpp"
#include "Reg.hpp"

class Syskey {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    public:

    static void     lock    ();
    static void     unlock  ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;
    static Irq ir;

    enum {
        SYSKEY = 0xBF803670,
            MAGIC1 = 0xAA996655,
            MAGIC2 = 0x556699AA,
        DMACON = 0xBF808900,
            DMASUSP = 1<<12
    };

};