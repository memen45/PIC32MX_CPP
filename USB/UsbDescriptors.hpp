#pragma once

//USB Descriptors

#include <cstdint>
#include "UsbBuf.hpp"

struct UsbDescriptors {

    //wValue=descriptor type,index (directly from setup packet)
    //buf = caller provided buffer, sz = size of buffer
    //return size of filled buffer, 0 if error
    static uint16_t get(uint16_t wValue, uint8_t* buf, uint16_t sz);

};
