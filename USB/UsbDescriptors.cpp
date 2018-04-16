#include "UsbDescriptors.hpp"
#include "UsbCh9.hpp"
#include "Usb.hpp"
#include <cstring> //strlen


//uint16_t-> byte, byte
#define BB(v) v bitand 0xFF, v >> 8

//create string header, from provided string-> uint16_t header
//strings are in uint16_t to provide wide char
constexpr uint16_t string_header(const char* str){
    return (UsbCh9::STRING<<8) bitor (strlen(str)*2+2);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DATA STARTS HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const bool self_powered = true;      //self-powered=1, bus-powered=0
static const bool remote_wakeup = false;    //remote wakeup enabled=1


//CDC-ACM
static const uint8_t device[] = {
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
    1                   //#of configurations
};

static const uint8_t config1[] = {
    //total size (2 bytes) after CONFIGURATION
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
    7, UsbCh9::ENDPOINT, UsbCh9::IN2, UsbCh9::BULK, BB(64), 0
};


static const uint16_t strings[] = {
   string_header("."),                  //use one . for each language
    0x0409,                             //language
   string_header("PIC32MM"),
    'P','I','C','3','2','M','M',        //manufacturer
   string_header("CDC-ACM"),
    'C','D','C','-','A','C','M'         //product
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DATA ENDS HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//private

//18bytes (but may ask for less)
uint8_t* get_device(uint16_t* siz){
    if(*siz > sizeof(device)) *siz = sizeof(device);
    return (uint8_t*)device;
}

//could be >255 bytes
uint8_t* get_config(uint16_t* siz, uint8_t idx){
    if(idx != 0){               //only 1 config
        *siz = 0;
        return 0;
    }
    if(*siz > sizeof(config1)) *siz = sizeof(config1);//limit siz
    return (uint8_t*)config1;
}

//up to 255bytes
uint8_t* get_string(uint16_t* siz, uint8_t idx){
    const uint8_t str_sz = sizeof(strings); //stay inside string array
    const uint8_t* str = (uint8_t*)strings; //convert to uint8_t*
    uint8_t i = 0;
    for(; i < str_sz and idx; i += str[i], idx-- ); //find idx
    if(i + str[i] > str_sz){                //not contained inside strings
        *siz = 0;
        return 0;
    }
    if(*siz > str[i]) *siz = str[i];    //limit siz to actual str size
    return (uint8_t*)&str[i];
}

//public

//pkt.wValue high=descriptor type, low=index
//device=1, config=2, string=3
uint8_t* UsbDescriptors::get(uint16_t wValue, uint16_t* siz){
    uint8_t idx = wValue;
    uint8_t typ = wValue>>8;
    if(typ == 1){ return get_device(siz); }
    if(typ == 2){ return get_config(siz, idx); }
    if(typ == 3){ return get_string(siz, idx); }
    return 0;
}

uint16_t UsbDescriptors::get_epsiz(uint8_t n, bool tr){
    if(n == 0) return device[7];
    if(tr) n += 128;    //if tx, set bit7
    for(uint8_t i = 0; i < sizeof(config1); i += config1[i]){
        if(config1[i+1] != UsbCh9::ENDPOINT or config1[i+2] != n) continue;
        return config1[i+4] bitor config1[i+5]<<8;
    }
    return 0;
}

uint8_t UsbDescriptors::get_epctrl(uint8_t n){
    if(n == 0) return Usb::EPTXEN|Usb::EPRXEN|Usb::EPHSHK;
    uint8_t ret = 0;
    for(uint8_t i = 0; i < sizeof(config1); i += config1[i]){
        if(config1[i+1] != UsbCh9::ENDPOINT) continue;
        if(config1[i+2] == n) ret or_eq Usb::EPRXEN;
        if(config1[i+2] == n+128) ret or_eq Usb::EPTXEN;
        if(config1[i+3] != UsbCh9::ISOCHRONOUS) ret or_eq Usb::EPHSHK;
    }
    return ret;
}

uint8_t UsbDescriptors::get_status(){
    return self_powered bitor (remote_wakeup<<1);
}
