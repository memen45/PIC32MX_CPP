//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DO NOT INCLUDE THIS FILE (except in Usb.hpp) - used inline in Usb.hpp
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


struct UsbBuf {
//______________________________________________________________________________
//
// all usb buffers come from here
// my_buffer_count * my_buffer_size byte buffers - all same length
// caller requests buffer via -
//   volatile uint8_t* mybuf = UsbBuf::get();
//   if(!mybuf) no_free_buffers- try again
// caller returns buffer when done
//   UsbBuf::release(mybuf);
// to reinit buffers (release all, and clear)-
//   UsbBuf::reinit();
//______________________________________________________________________________

    static void                 reinit();
    static volatile uint8_t*    get();
    static void                 release(volatile uint8_t*);
    static uint8_t              buf_len();

    typedef struct {
        bool inuse;
        uint8_t buf[my_buffer_size];
    } buf_t;
};
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// UsbBuf static functions, vars
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
static volatile UsbBuf::buf_t m_buffers[my_buffer_count] = {0};

//clear buffers, clear inuse flag
void UsbBuf::reinit(){
    for(auto& i : m_buffers){
        i.inuse = false;
        for(auto& j : i.buf) j = 0;
    }
}
//get an unused buffer (address), return 0 if none available (caller checks 0)
volatile uint8_t* UsbBuf::get(){
    for(auto& i : m_buffers){
        if(i.inuse) continue;
        i.inuse = true;
        return i.buf;
    }
    return 0;
}
//return a buffer for use
void UsbBuf::release(volatile uint8_t* p){
    for(auto& i : m_buffers){
        if(p!=i.buf) continue;
        i.inuse = false;
        return;
    }
}
//size of buffer
uint8_t UsbBuf::buf_len() { return my_buffer_size; }
//______________________________________________________________________________
