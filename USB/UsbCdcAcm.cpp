#include "UsbCdcAcm.hpp"
#include "UsbCh9.hpp"
#include <cstring> //strlen

//uint16_t-> byte, byte
#define BB(v) v bitand 0xFF, v >> 8
//string header (size, type)
#define SHDR(v) (strlen(v)*2+2), UsbCh9::STRING


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DATA STARTS HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//CDC-ACM
static const uint8_t m_descriptor[] = {
    18,                 //length(always)
    UsbCh9::DEVICE,     //1
    BB(0x0101),         //0x0200 //(bcd) usb 1.1 (prevent unneeded inquiries)
    2,                  //class (CDC device)
    0,                  //subclass
    0,                  //protocol
    64,                 //max ep0 packet size (IN/OUT same)
    BB(0x04D8),         //VID
    BB(0x000A),         //PID
    BB(0x0100),         //(bcd) release number
    1,                  //string index- manufacturer
    2,                  // -product
    0,                  // -serial number
    1,                  //#of configurations

    //==== config 1 ====

    //total size of this configuration (2 bytes) after CONFIGURATION
    9, UsbCh9::CONFIGURATION, BB((9+9+5+4+5+5+7+9+7+7)), 2, 1, 0,
        UsbCh9::SELFPOWER, 50,

    //interface0
    9, UsbCh9::INTERFACE, 0, 0, 1, 2, 2, 1, 0,

    //cdc descriptors
    5, 36, 0, BB(0x110),    //cdc header
    4, 36, 2, 0,            //acm, the last is D0|D1|D2|D3- don't need
    5, 36, 6, 0, 1,         //union comm id=0, data id=1
    5, 36, 1, 0, 1,         //call management

    //endpoint  //8,0 = 0x0008
    7, UsbCh9::ENDPOINT, UsbCh9::IN1, UsbCh9::INTERRUPT, BB(8), 2,

    //interface1
    9, UsbCh9::INTERFACE, 1, 0, 2, 10, 0, 0, 0,

    //endpoint
    7, UsbCh9::ENDPOINT, UsbCh9::OUT2, UsbCh9::BULK, BB(64), 0,

    //endpoint
    7, UsbCh9::ENDPOINT, UsbCh9::IN2, UsbCh9::BULK, BB(64), 0,

    //==== strings ====

    4, UsbCh9::STRING, 9, 4, //language 0x0409

    SHDR("PIC32MM"),
        'P',0,'I',0,'C',0,'3',0,'2',0,'M',0,'M',0, //manufacturer

    SHDR("CDC-ACM"),
        'C',0,'D',0,'C',0,'-',0,'A',0,'C',0,'M',0,  //product

    0 //end of descriptor marker - NEED to know where descriptor ends
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DATA ENDS HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static UsbEP m_ep_state;  //ep1 = serial state
static UsbEP m_ep_txrx;   //ep2 = tx/rx

//=============================================================================
    bool        UsbCdcAcm::init         (bool tf)
//=============================================================================
{
    UsbDescriptors::set_device(m_descriptor, service);
    bool ret = UsbDevice::init(tf);
    m_ep_state.init(1);
    m_ep_txrx.init(2);
    return ret;
}

//=============================================================================
    bool        UsbCdcAcm::service      (uint32_t, uint8_t)
//=============================================================================
{
    return true;
}

