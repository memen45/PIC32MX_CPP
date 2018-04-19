#include "UsbDescriptors.hpp"
#include "UsbCh9.hpp"
#include "Usb.hpp"

//=============================================================================
const uint8_t*              UsbDescriptors::m_descriptor = 0;
UsbDescriptors::service_t   UsbDescriptors::m_service = 0;
uint8_t                     UsbDescriptors::current_config = 1; //config 1
//=============================================================================



//=============================================================================
    bool            UsbDescriptors::service (uint32_t flags, uint8_t ustat)
//=============================================================================
{
    if(m_service) return m_service(flags, ustat);
    return false;
}

//pkt.wValue high=descriptor type, low=index
//device=1, config=2, string=3
//=============================================================================
    const uint8_t*  UsbDescriptors::get_desc (uint16_t wValue, uint16_t* siz)
//=============================================================================
{
    if(not m_descriptor){ *siz = 0; return 0; }
    uint8_t idx = wValue;
    uint8_t typ = wValue>>8;
    uint16_t i = 0;
    //find header type, if idx > 0, also find header[idx]
    for(;;){
         if(m_descriptor[i+1] == typ){
             if(not idx) break; //found it
             idx--; //wrong index, dec until 0
         }
         i += m_descriptor[i];
         if(not i){ *siz = 0; return 0; } //at end of descriptor (0 marker)
    }
    //we now have index into descriptor
    //get size of type, if config get total size
    uint16_t actsiz = typ == UsbCh9::CONFIGURATION ?
        m_descriptor[i+2] + (m_descriptor[i+3]<<8) : //total size of config
        m_descriptor[i]; //size of others
    //limit to actual size
    if(*siz > actsiz) *siz = actsiz;
    return &m_descriptor[i];
}

//set to descriptor wanted (set before usb init)
//=============================================================================
    void    UsbDescriptors::set_device      (const uint8_t * d, service_t f)
//=============================================================================
{
    m_descriptor = d;
    m_service = f;
}

//do nothing, return descriptor ptr for config if found, 0 if not
//=============================================================================
    const uint8_t*  UsbDescriptors::set_config  (uint16_t wValue)
//=============================================================================
{
    wValue and_eq 0xFF;                             //low byte only
    if(not wValue) return 0;                        //no 0 config#
    uint16_t wv = UsbCh9::CONFIGURATION<<8;         //get first config
    uint16_t siz;                                   //needed, not used
    for(;; wv++){                                   //until config# match
        const uint8_t* p = get_desc(wv, &siz);      //get pointer to config
        if(not p) break;                            //no more configs left
        if(p[5] == wValue){                         //matching number?
            current_config = wValue;                //save it
            return p;                               //return pinter to config
        }
    }
    return 0;                                       //bad config#
}

//=============================================================================
    uint16_t    UsbDescriptors::get_epsiz       (uint8_t n, bool tr)
//=============================================================================
{
    if(not m_descriptor) return 0;
    if(n == 0) return m_descriptor[7];                //tx/rx same for ep0
    if(tr) n += 128;                                //if tx, set bit7
    const uint8_t* p = set_config(current_config);  //get config offset
    for(; p[1] != UsbCh9::STRING; p += p[0]){       //until into STRINGs
        if(p[1] != UsbCh9::ENDPOINT or m_descriptor[2] != n) continue;
        return p[4] bitor (p[5]<<8);                //found ep, return size
    }
    return 0;                                       //not found
}

//=============================================================================
    uint8_t     UsbDescriptors::get_epctrl      (uint8_t n)
//=============================================================================
{
    if(not m_descriptor) return 0;
    if(n == 0) return Usb::EPTXEN|Usb::EPRXEN|Usb::EPHSHK; //ep0 always this
    uint8_t ret = 0;
    const uint8_t* p = set_config(current_config);  //get config offset
    for(; p[1] != UsbCh9::STRING; p += p[0]){       //until into STRINGs
        if(p[1] != UsbCh9::ENDPOINT) continue;      //not an endpoint, continue
        if(p[2] == n) ret or_eq Usb::EPRXEN;        //if rx, enable rx
        if(p[2] == n+128) ret or_eq Usb::EPTXEN;    //if tx, nebale tx
        //enable handshake unless isochronous
        if(p[3] != UsbCh9::ISOCHRONOUS) ret or_eq Usb::EPHSHK;
    }
    return ret;
}

//=============================================================================
    uint8_t     UsbDescriptors::get_status      ()
//=============================================================================
{
    if(not m_descriptor) return 0;
    const uint8_t* p = set_config(current_config);  //get config offset
    uint8_t ret = 0;
    if(p[7] bitand 0x40) ret = 1;                   //self-powered
    if(p[7] bitand 0x20) ret or_eq 2;               //remote wakeup
    return ret;
}
