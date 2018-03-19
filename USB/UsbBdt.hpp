#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "UsbConfig.hpp"

struct UsbBdt {

    //USB Buffer Descriptor Control Format
    using ctrl_t = struct {
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
        uint32_t bufaddr;
    };
    //USB Buffer Descriptor Status Format
    using stat_t = struct {
        unsigned        :2;
        unsigned pid    :4;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        uint32_t bufaddr;
    };
    using bdt_t = union {ctrt_t ctrl; stat_t stat;};

    //ep0 rx even   ctrl|stat,addr 2words/8bytes table[0]
    //ep0 rx odd    ctrl|stat,addr 2words/8bytes table[1]
    //ep0 tx even   ctrl|stat,addr 2words/8bytes table[2]
    //ep0 tx odd    ctrl|stat,addr 2words/8bytes table[3]
    //...

    static volatile bdt_t table[(UsbConfig::last_ep_num+1)*4]
        __attribute__ ((aligned (512)));

    //access buffer descriptor info-
    //endpoint/dir/ppbi (which is index into table, 0-63)
    //Usb::stat() can be used for that value (is already shifted value)
    //eg- UsbBdt::table[Usb::stat()].uown = 1;
    //eg- UsbBdt::table[stat].uown = 1;

    //access bufaddr via functions, as they also convert to/from physical address
    //eg- UsbBdt::bufaddr(0, mybuffer); //ep0,rx,even
    static void         bufaddr     (uint8_t, uint8_t*);    //set buffer address
    static uint8_t*     bufaddr     (uint8_t);              //get buffer address


};

