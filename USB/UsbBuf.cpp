#include <cstdint>
#include "UsbBuf.hpp"

uint32_t UsbBuf::m_status = 0;
uint8_t UsbBuf::m_buffer64[m_buffer64_n] = {0};

//=============================================================================
    uint8_t*        UsbBuf::get64           ()
//=============================================================================
{
    uint32_t i = 1, j = 0;
    for( ; j < m_buffer64_n; i <<= 1, j++){
        if(m_status bitand i) continue;         //is in use
        m_status or_eq i;                       //else set to inuse
        return &m_buffer64[j];                  //give
    }
    return 0;                                   //nothing available
}

//=============================================================================
    void        UsbBuf::release        (uint8_t* p)
//=============================================================================
{
    uint32_t i = 1, j = 0;
    for( ; j < m_buffer64_n; i <<= 1, j++){
        if(p not_eq &m_buffer64[j]) continue;   //no match
        m_status and_eq compl i;                //match, set to not inuse
        m_buffer64[j] = {0};                    //clear it (not necessary)
        return;
    }
}

//=============================================================================
    void        UsbBuf::init            ()
//=============================================================================
{
    m_status = 0;
    m_buffer64 = {0};   //clear everything (really only need to clear .status)
                        //only takes <20us/kb @24MHz to clear everything
}
