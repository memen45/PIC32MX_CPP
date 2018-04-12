#pragma once

//USB peripheral - PIC32MM0256GPM064

#include "UsbConfig.hpp"
#include <cstdint>

struct UsbBdt {

    //USB Buffer Descriptor Control Format
    using ctrl_t = union {
        struct {
        unsigned        :2;
        unsigned bstall :1;
        unsigned dts    :1;
        unsigned ninc   :1;
        unsigned keep   :1;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        };
        uint8_t val8;
        uint32_t val32;
    };
    //USB Buffer Descriptor Status Format
    using stat_t = union {
        struct {
        unsigned        :2;
        unsigned pid    :4;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        };
        uint8_t val8;
        uint32_t val32;
    };

    using bdt_t = struct {
        union { ctrl_t ctrl; stat_t stat; };
        uint32_t bufaddr;
    };

    static volatile bdt_t table[UsbConfig::last_ep_num+1][2][2]
        __attribute__ ((aligned (512)));

    //clear table
    static void         init        ();

};






