#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "UsbConfig.hpp"

struct UsbBdt {

    //bdt table

    // Buffer Descriptor Status Register layout.
    using BD_STAT = union {
        struct {
            unsigned            :2;
            unsigned    BSTALL  :1;
            unsigned    DTSEN   :1;
            unsigned            :2;
            unsigned    DTS     :1;
            unsigned    UOWN    :1;
        };
        struct {
            unsigned            :2;
            unsigned    PID0    :1;
            unsigned    PID1    :1;
            unsigned    PID2    :1;
            unsigned    PID3    :1;

        };
        struct {
            unsigned            :2;
            unsigned    PID     :4;
        };
        uint16_t Val;
    };

    // BDT Entry Layout
    using BDT_ENTRY = union {
        struct {
            BD_STAT     STAT;
            uint16_t    CNT     :10;
            uint32_t    ADR;
        };
        struct {
            uint32_t    res     :16;
            uint32_t    count   :10;
        };
        uint32_t w[2];
        uint16_t v[4];
        uint64_t Val;
    };


    volatile BDT_ENTRY table[UsbConfig::last_ep_num+1]
        __attribute__ ((aligned (512)));

    //clear table
    static void     init        ();


};

