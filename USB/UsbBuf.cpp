#include <cstdint>
#include "UsbBuf.hpp"

uint32_t UsbBuf::m_status = 0;
uint8_t UsbBuf::m_buffer64[m_buffer64_n][64] = {0};
uint8_t UsbBuf::m_buffer512[512] = {0};

//=============================================================================
    uint8_t*        UsbBuf::get64           ()
//=============================================================================
{
    uint32_t i = 1, j = 0;
    for( ; j < m_buffer64_n; i <<= 1, j++){
        if(m_status bitand i) continue;         //is in use
        m_status or_eq i;                       //else set to inuse
        return &m_buffer64[j][0];               //give
    }
    return 0;                                   //nothing available
}

//=============================================================================
    uint8_t*        UsbBuf::get512          ()
//=============================================================================
{
    if(m_status bitand (1<<31)) return 0;
    m_status or_eq (1<<31);
    return m_buffer512;
}

//=============================================================================
    void        UsbBuf::release             (uint8_t* p)
//=============================================================================
{
    uint32_t i = 1, j = 0;
    for( ; j < m_buffer64_n; i <<= 1, j++){
        if(p < &m_buffer64[j][0] or p > &m_buffer64[j][63]) continue;//no match
        m_status and_eq compl i;                //match, set to not inuse
        return;
    }
    if(p >= &m_buffer512[0] and p <= &m_buffer512[511]){
        m_status and_eq compl (1<<31);
    }
}

//=============================================================================
    void        UsbBuf::init            ()
//=============================================================================
{
    m_status = 0;
    //clear everything (really only need to clear .status)
    //takes <20us/kb @24MHz
    for(auto& i : m_buffer512) i = 0;
    for(auto& i : m_buffer64){
        for(auto& ii : i) ii = 0;
    }
}
