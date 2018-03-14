#pragma once

//USB Descriptors

#include <cstdint>
#include "UsbBuf.hpp"

struct UsbDescriptors {

    static void get(uint16_t wValue, UsbBuf::buffer64_t& buf);


//    private:
//    //caller provides buffer, buffer size
//    static void get_device(uint8_t* buf, uint16_t& sz);
//
//    //caller provides buffer, buffer size, string index wanted
//    static void get_string(uint8_t* buf, uint16_t& sz, uint8_t idx);
//
//    //caller provides buffer, buffer size, config index wanted
//    static void get_config(uint8_t* buf, uint16_t& sz, uint8_t idx);

    //sz will be set to number of bytes placed in buffer
    //or 0 if index out of range or buffer too small
};
