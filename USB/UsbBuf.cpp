#include <cstdint>
#include "UsbBuf.hpp"


UsbBuf::m_buffer_t UsbBuf::m_buffers = {0};

//=============================================================================
    auto        UsbBuf::get64           () -> buffer64_t*
//=============================================================================
{
    for(auto i = 0; i < m_buffer64_n; i++){
        if(m_buffers.status bitand (1<<i)) continue;
        m_buffers.status or_eq 1<<i; //set to inuse
        m_buffers.buffer64[i].buf_size = 64; //set size
        return &m_buffers.buffer64[i];
    }
    return 0;
}

//=============================================================================
    auto        UsbBuf::get1024         () -> buffer1024_t*
//=============================================================================
{
    for(auto i = m_buffer64_n; i < m_buffer64_n+m_buffer1024_n; i++){
        if(m_buffers.status bitand (1<<i)) continue;
        m_buffers.status or_eq 1<<i; //set to inuse
        i >>= m_buffer64_n;
        m_buffers.buffer1024[i].buf_size = 1024;
        return &m_buffers.buffer1024[i];
    }
    return 0;
}

//=============================================================================
    void        UsbBuf::release        (void* bufp)
//=============================================================================
{
    for(auto i = 0; i < m_buffer64_n; i++){
        if(bufp not_eq (void*)&m_buffers.buffer64[i]) continue;
        m_buffers.status and_eq compl(1<<i); //set to not inuse
        return;
    }
    for(auto i = m_buffer64_n; i < m_buffer64_n+m_buffer1024_n; i++){
        if(bufp not_eq (void*)&m_buffers.buffer1024[i>>m_buffer64_n]) continue;
        m_buffers.status and_eq compl(1<<i); //set to not inuse
        return;
    }
}

//=============================================================================
    void        UsbBuf::reinit         ()
//=============================================================================
{
    m_buffers.status = 0;
}
