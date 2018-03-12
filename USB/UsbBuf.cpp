#include <cstdint>
#include "UsbBuf.hpp"


UsbBuf::m_buffer_t UsbBuf::m_buffers = {0};

//=============================================================================
    auto        UsbBuf::get64           () -> buffer64_t*
//=============================================================================
{
    uint32_t i = 1, j = 0;
    for( ; j < m_buffer64_n; i <<= 1, j++){
        if(m_buffers.status bitand i) continue; //is in use
        m_buffers.status or_eq i;               //else set to inuse
        m_buffers.buffer64[j].buf_size = 64;    //and set size
        return &m_buffers.buffer64[j];          //give
    }
    return 0;                                   //nothing available
}

//=============================================================================
    auto        UsbBuf::get1024         () -> buffer1024_t*
//=============================================================================
{
    uint32_t i = 1<<m_buffer64_n, j = 0;
    for( ; j < m_buffer1024_n; i <<= 1, j++){
        if(m_buffers.status bitand i) continue; //is in use
        m_buffers.status or_eq i;               //else set to inuse
        m_buffers.buffer1024[j].buf_size = 1024;//and set size
        return &m_buffers.buffer1024[j];        //give
    }
    return 0;                                   //nothing available
}

//=============================================================================
    void        UsbBuf::release        (void* bufp)
//=============================================================================
{
    uint32_t i = 1, j = 0;
    for( ; j < m_buffer64_n; i <<= 1, j++){
        if(bufp not_eq (void*)&m_buffers.buffer64[j]) continue; //no match
        m_buffers.status and_eq compl i;        //match, set to not inuse
        m_buffers.buffer64[j] = {0};            //clear it (not necessary)
        return;
    }
    for( ; j < m_buffer1024_n; i <<= 1, j++){
        if(bufp not_eq (void*)&m_buffers.buffer1024[j]) continue; //no match
        m_buffers.status and_eq compl i;        //match, set to not inuse
        m_buffers.buffer1024[j] = {0};          //clear it (not necessary)
        return;
    }
}

//=============================================================================
    void        UsbBuf::reinit         ()
//=============================================================================
{
    m_buffers = {0};    //clear everything (really only need to clear .status)
                        //only takes <20us/kb @24MHz to clear everything
}
