#pragma once

#include <cstdint>

/*
    general buffers for usb - can get/release as needed
    2 fixed sizes - 64bytes, 1024bytes
    given as a struct pointer buffer64_t* or buffer1024_t*,
    (returns 0 is none available)
    the struct includes some extras for the
    caller to use as needed (keep track of size, etc.)

    send struct pointer back to release() to give up the buffer

    Usb::attach() will clear all buffers and status

*/

struct UsbBuf {

    //callers use
    using buffer64_t = struct {
        uint16_t    buf_size;   //will be set to size of this buffer, caller can use
        uint32_t    status;     //caller can use
        void*       vp;         //caller can use
        uint8_t     buf[64];    //the buffer
    };

    //callers use (used same as 64, but has 1024byte buffer)
    using buffer1024_t = struct {
        uint16_t    buf_size;
        uint32_t    status;
        void*       vp;
        uint8_t     buf[1024];
    };

    static buffer64_t*      get64   ();             //get 64bytes (0 if fails)
    static buffer1024_t*    get1024 ();             //get 1024bytes (0 if fails)
    static void             release (void* bufp);   //release buffer
    static void             reinit  ();             //clear status (all now free)

    private:

    //up to 32 buffers (using uint32_t for in use flag)
    static const uint8_t m_buffer64_n = 16;         //number of 64byte buffers
    static const uint8_t m_buffer1024_n = 4;        //number of 1024byte buffers

    //internal storage/tracking
    using m_buffer_t = struct {
        uint32_t        status; //inuse bits - 0=available 1=used
        buffer64_t      buffer64[m_buffer64_n];         //64byte buffers
        buffer1024_t    buffer1024[m_buffer1024_n];     //1024byte buffers
    };

    static m_buffer_t m_buffers;

};
