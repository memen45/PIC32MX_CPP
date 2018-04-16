#pragma once

//USB Descriptors

#include "UsbCh9.hpp"
#include <cstdint>

struct UsbDescriptors {

    //wValue=descriptor type,index (directly from setup packet)
    //siz = requested size, change to actual size in function if smaller
    //return pointer to wanted bytes
    static uint8_t* get(uint16_t wValue, uint16_t* siz);

    //set configuration (return index into config- 1=bad index)
    static uint16_t set_config(uint16_t wValue);

    //get ep size from descriptor/config
    static uint16_t get_epsiz(uint8_t n, bool tr);

    //get endpoint control info based on descriptor info
    static uint8_t get_epctrl(uint8_t n);

    //self-powered, remote wakeup
    static uint8_t get_status();


    static const uint8_t last_ep_num = 2;

    static uint16_t current_config;

};
