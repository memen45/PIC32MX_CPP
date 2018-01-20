
#include "Reg.hpp"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DO NOT INCLUDE THIS FILE (except in Usb.hpp) - used inline in Usb.hpp
// do something better later
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


struct UsbBuf {
/*

 all usb buffers come from here
 my_buffer_count * my_buffer_size byte buffers - all same length
 caller requests buffer via -
   volatile uint8_t* mybuf = UsbBuf::get();
   if(!mybuf) no_free_buffers- try again
 caller returns buffer when done
   UsbBuf::release(mybuf);
 to reinit buffers (release all, and clear)-
   UsbBuf::reinit();
*/

    static void                 reinit();
    static uint8_t*             get();
    static void                 release(uint8_t*);
    static uint8_t              buf_len();

    typedef struct {
        bool inuse;
        uint8_t buf[my_buffer_size];
    } buf_t;
};

static UsbBuf::buf_t m_buffers[my_buffer_count] = {0};

//clear buffers, clear inuse flag
//=============================================================================
    void        UsbBuf::reinit          ()
//=============================================================================
{
    for(auto& i : m_buffers){
        i.inuse = false;
        for(auto& j : i.buf) j = 0;
    }
}

//get an unused buffer (address), return 0 if none available (caller checks 0)
//=============================================================================
    uint8_t*    UsbBuf::get             ()
//=============================================================================
{
    for(auto& i : m_buffers){
        if(i.inuse) continue;
        i.inuse = true;
        return i.buf;
    }
    return 0;
}

//return a buffer for use
//=============================================================================
    void        UsbBuf::release         (uint8_t* p)
//=============================================================================
{
    //in case was physical address from bdt
    //convert to kseg0
    p = (uint8_t*)Reg::p2kseg0(p);
    for(auto& i : m_buffers){
        if(p!=i.buf) continue;
        i.inuse = false;
        return;
    }
}

//size of buffer
//=============================================================================
    uint8_t     UsbBuf::buf_len         ()
//=============================================================================
{
    return my_buffer_size;
}







struct UsbBuf2 {

    //callers use
    typedef struct {
        uint16_t buf_size;//will be set to size of this buffer, caller can use
        uint32_t status;  //caller can use
        void* vp;         //caller can use (callback function pointer)
        uint8_t buf[64];  //the buffer
    } buffer64_t;

    //callers use (used same as 64, but has 512byte buffer)
    typedef struct {
        uint16_t buf_size;
        uint32_t status;
        void* vp;
        uint8_t buf[512];
    } buffer512_t;

    static buffer64_t*      get64   ();             //get 64bytes (*0 if fails)
    static buffer512_t*     get512  ();             //get 512bytes (*0 if fails)
    static void             release (void* bufp);   //release buffer
    static void             reinit  ();             //clear status (all now free)

    //internal storage/tracking
    typedef struct {
        uint32_t status; //bit n 0-15  if(1<<n) inuse, bit n 16-19 if(1<<n) inuse
        buffer64_t buffer64[16];
        buffer512_t buffer512[4];
    } m_buffer_t;

};

static UsbBuf2::m_buffer_t m_buffers2 = {0};

//=============================================================================
    auto        UsbBuf2::get64          () -> buffer64_t*
//=============================================================================
{
    for(auto i = 0; i < 16; i++){
        if(m_buffers2.status & (1<<i)) continue;
        m_buffers2.status |= 1<<i; //inuse
        m_buffers2.buffer64[i].buf_size = 64;
        return &m_buffers2.buffer64[i];
    }
    return 0;
}

//=============================================================================
    auto        UsbBuf2::get512         () -> buffer512_t*
//=============================================================================
{
    for(auto i = 16; i < 20; i++){
        if(m_buffers2.status & (1<<i)) continue;
        m_buffers2.status |= 1<<i; //inuse
        i >>= 16;
        m_buffers2.buffer512[i].buf_size = 512;
        return &m_buffers2.buffer512[i];
    }
    return 0;
}

//=============================================================================
    void        UsbBuf2::release        (void* bufp)
//=============================================================================
{
    for(auto i = 0; i < 16; i++){
        if(bufp != (void*)&m_buffers2.buffer64[i]) continue;
        m_buffers2.status &= ~(1<<i); //not inuse
        return;
    }
    for(auto i = 16; i < 20; i++){
        if(bufp != (void*)&m_buffers2.buffer512[i>>16]) continue;
        m_buffers2.status &= ~(1<<i); //not inuse
    }
}

//=============================================================================
    void        UsbBuf2::reinit         ()
//=============================================================================
{
    m_buffers2.status = 0;
}