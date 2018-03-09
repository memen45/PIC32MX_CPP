#pragma once

#include <cstdint>


struct UsbBuf {

    //callers use
    using buffer64_t = struct {
        uint16_t buf_size;//will be set to size of this buffer, caller can use
        uint32_t status;  //caller can use
        void* vp;         //caller can use (callback function pointer)
        uint8_t buf[64];  //the buffer
    };

    //callers use (used same as 64, but has 1024byte buffer)
    using buffer1024_t = struct {
        uint16_t buf_size;
        uint32_t status;
        void* vp;
        uint8_t buf[1024];
    };

    static buffer64_t*      get64   ();             //get 64bytes (*0 if fails)
    static buffer1024_t*    get1024 ();             //get 1024bytes (*0 if fails)
    static void             release (void* bufp);   //release buffer
    static void             reinit  ();             //clear status (all now free)

    private:

    static const uint8_t m_buffer64_n = 16;
    static const uint8_t m_buffer1024_n = 4;

    //internal storage/tracking
    using m_buffer_t = struct {
        uint32_t status; //bit n 0-15  if(1<<n) inuse, bit n 16-19 if(1<<n) inuse
        buffer64_t buffer64[m_buffer64_n];
        buffer1024_t buffer1024[m_buffer1024_n];
    };

    static m_buffer_t m_buffers;

};
