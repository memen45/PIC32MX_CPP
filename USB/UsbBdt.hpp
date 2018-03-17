#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "UsbConfig.hpp"

struct UsbBdt {

    using bdt_t = struct {uint32_t ctrlstat; uint32_t bufaddr; };

    volatile bdt_t table[(UsbConfig::last_ep_num+1)*4]
        __attribute__ ((aligned (512)));

    //clear table
    static void         init        ();

    static void         bufaddr     (uint8_t, uint32_t);    //set buffer address
    static uint8_t      bufaddr     (uint8_t);              //get buffer address

    static void         count       (uint8_t, uint16_t);    //set count (ctrl)
    static uint16_t     count       (uint8_t);              //get count (stat)

    static void         uown        (uint8_t, bool);        //set uown (ctrl)
    static bool         uown        (uint8_t);              //get uown (stat)

    static void         data01      (uint8_t, bool);        //set data01 (ctrl)
    static bool         data01      (uint8_t);              //get data01 (stat)

    static void         keep        (uint8_t, bool);        //set keep (ctrl)
    static void         ninc        (uint8_t, bool);        //set ninc (ctrl)
    static void         dts         (uint8_t, bool);        //set dts (ctrl)
    static void         bstall      (uint8_t, bool);        //set bstall (ctrl)

    static uint8_t      pid         (uint8_t);              //get pid (stat)

};

