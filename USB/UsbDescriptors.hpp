#pragma once

//USB Descriptors

#include <cstdint>

struct UsbDescriptors {

    //wValue=descriptor type,index (directly from setup packet)
    //buf = buffer to copy info to
    //siz = requested size, change to actual size in function if smaller
    //return size of bytes
    static uint16_t get(uint16_t wValue, uint8_t* buf, uint16_t siz);

    //get ep size from descriptor/config (epn=endpoint, io= 0=out, 1=in)
    static uint16_t get_epsiz(uint8_t epn, bool io);
};
