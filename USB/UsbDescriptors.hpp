#pragma once

//USB Descriptors

#include <cstdint>

struct UsbDescriptors {

    //strings copied to use provided buffer, so we don't have to
    //otherwise prefill various lengths into a constant
    //eg- strings, we can have 'normal' strings and make them
    //wide chars and calc then length at runtime

    //wValue=descriptor type,index (directly from setup packet)
    //buf = caller provided buffer, sz = max size (wLength)
    //return size of filled buffer, 0 if error
    static uint16_t get(uint16_t wValue, uint8_t* buf, uint16_t sz);

};
