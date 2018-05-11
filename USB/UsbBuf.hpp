#pragma once

#include <cstdint>

/*
    general buffers for usb - can get/release as needed
    fixed size - 64bytes
    given as a uint8_t* (returns 0 if none available)

    send pointer back to release() to give up the buffer
    (any pointer address inside the buffer will work)

    UsbBuf::init() will be called when usb attached/detached
    which clears buffers and status


    examples-

    uint8_t* mybuffer = UsbBuf::get64();
    if(mybuffer){ is good  } else { none available }

    UsbBuf::release(mybuffer);
    mybuffer = 0;

*/

struct UsbBuf {

    static uint8_t*         get64       ();
    static void             release     (uint8_t*);
    static void             init        ();

    private:

    //up to 32 buffers (using uint32_t for in use flag)
    static const uint8_t m_buffer64_n = 16; //number of 64byte buffers


    //internal storage/tracking
    static uint32_t     m_status;   //inuse bits - 0=available 1=used
    static uint8_t      m_buffer64[m_buffer64_n][64] //64byte buffers
        __attribute__ ((aligned (4)));

};
