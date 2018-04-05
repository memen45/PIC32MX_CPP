#include "UsbDescriptors.hpp"
#include "UsbCh9.hpp"
#include <cstring> //memcpy


//uint16_t-> byte, byte
#define BB(v) v bitand 0xFF, v >> 8
// //uint8_t,uin8_t-> uint16_t
// #define W(b1,b2) (b2<<8 bitor b1)


//CDC-ACM
static const uint8_t device[] = {
    18,                 //length
    UsbCh9::DEVICE,     //1
    BB(0x0101),         //0x0200 //(bcd) usb 2.0 (changed to 1.1)
    2,                  //class (CDC device)
    0,                  //subclass
    0,                  //protocol
    64,                 //max ep0 packet size
    BB(0x04D8),         //VID
    BB(0x000A),         //PID
    BB(0x0100),         //(bcd) release number
    1,                  //string index- manufacturer
    2,                  // -product
    0,                  // -serial number
    1                   //#of configurations
};

static const uint8_t config1[] = {
    //total size in third,fourth byte
    9, UsbCh9::CONFIGURATION, BB(0), 2, 1, 0, UsbCh9::SELFPOWER, 50,

    //interface0
    9, UsbCh9::INTERFACE, 0, 0, 1, 2, 2, 1, 0,
    //cdc descriptors
    5, 36, 0, 16, 1,        //cdc header
    4, 36, 2, 0,            //acm, the last is D0|D1|D2|D3- don't need
    5, 36, 6, 0, 1,         //union comm id=0, data id=1
    5, 36, 1, 0, 1,         //call management
    //endpoint  //8,0 = 0x0008
    7, UsbCh9::ENDPOINT, UsbCh9::IN1, UsbCh9::INTERRUPT, 8, 0, 2,

    //interface1
    9, UsbCh9::INTERFACE, 1, 0, 2, 10, 0, 0, 0,
    //endpoint //64,0 = 0x0040
    7, UsbCh9::ENDPOINT, UsbCh9::OUT2, UsbCh9::BULK, BB(64), 0,
    //endpoint
    7, UsbCh9::ENDPOINT, UsbCh9::IN2, UsbCh9::BULK, BB(64), 0
};

//Language code string descriptor
static const char* strings[] = {
   "\x09\x04",                     //language (low byte, high byte)
   "Microchip Techonology Inc.",   //manufacturer
   "CDC RS-232 Emulation Demo"     //product
};

//private

//18bytes (but may ask for less)
uint16_t get_device(uint8_t* buf, uint16_t siz){
    if(siz > device[0]) siz = device[0];
    memcpy(buf, device, siz);
    return siz;
}

//could be >255 bytes
uint16_t get_config(uint8_t* buf, uint16_t siz, uint8_t idx){
    if(idx != 0) return 0;              //only 1 config
    uint16_t cfgsiz = sizeof(config1);  //get length of config1
    if(siz > cfgsiz) siz = cfgsiz;      //limit siz
    memcpy(buf, config1, siz);          //copy to buf, amount requested
    buf[2] = cfgsiz;                    //fill in total config size
    buf[3] = cfgsiz>>8;                 //upper byte
    return siz;                         //size requested or config1 size
}

//up to 255bytes
uint16_t get_string(uint8_t* buf, uint16_t siz, uint8_t idx){
    if(idx >= sizeof(strings)) return 0;    //bad index
    const char* str = strings[idx];         //get string pointer
    auto slen = 2 + (strlen(str)<<(idx?1:0)); //get str len (*2 if wide), +2
    if(slen < siz) siz = slen;              //limit siz
    buf[0] = slen;                          //fill in string length
    buf[1] = UsbCh9::STRING;                //and type
    if(idx == 0){                           //language id, as-is
        memcpy(&buf[2], str, siz-2);
    } else {
        for(uint16_t i = 2; i < siz-1; ){   //is normal string,
            buf[i++] = *str++;              //need to make wchar
            buf[i++] = 0;                   //wide char
        }
    }
    return siz;
}

//public

//pkt.wValue high=descriptor type, low=index
//device=1, config=2, string=3
uint16_t UsbDescriptors::get(uint16_t wValue, uint8_t* buf, uint16_t siz){
    uint8_t idx = wValue;
    uint8_t typ = wValue>>8;
    if(typ == 1){ return get_device(buf, siz); }
    if(typ == 2){ return get_config(buf, siz, idx); }
    if(typ == 3){ return get_string(buf, siz, idx); }
    return 0;
}


// static const uint16_t language[] = {
//     W(4,3), 0x0409
// };
// static const uint16_t manufacturer[] = {
//     W(36,3), 'P','I','C','3','2','M','M','0','2','5','6','G','P','M','0','6','4'
// };
// static const uint16_t product[] = {
//     W(16,3), 'C','D','C','-','A','C','M'
// };
// static const uint16_t* strings[] = {
//     language, manufacturer, product
// };

//private

//18bytes (but may ask for less)
// const uint8_t* get_device(uint16_t* siz){
//     if(*siz > sizeof(device)) *siz = sizeof(device);
//     return device;
// }
//
// //could be >255 bytes
// const uint8_t* get_config(uint16_t* siz, uint8_t idx){
//     if(idx != 0){ *siz = 0; return 0; } //only 1 config
//     if(*siz > sizeof(config1)) *siz = sizeof(config1);
//     return config1;
// }
//
// //up to 255bytes
// const uint8_t* get_string(uint16_t* siz, uint8_t idx){
//     if(idx >= sizeof(strings)){ *siz = 0;  return 0; } //bad index
//     uint8_t* str = (uint8_t*)strings[idx];
//     auto ssiz = str[0];
//     if(*siz > ssiz) *siz = ssiz;
//     return (const uint8_t*)strings[idx];
// }
//
// //public
//
// //pkt.wValue high=descriptor type, low=index
// //device=1, config=2, string=3
// uint8_t* UsbDescriptors::get(uint16_t wValue, uint16_t* siz){
//     uint8_t idx = wValue;
//     uint8_t typ = wValue>>8;
//     if(typ == 1){ return (uint8_t*)get_device(siz); }
//     if(typ == 2){ return (uint8_t*)get_config(siz, idx); }
//     if(typ == 3){ return (uint8_t*)get_string(siz, idx); }
//     *siz = 0;
//     return 0;
// }

uint16_t UsbDescriptors::get_epsiz(uint8_t n, bool io){
    if(n == 0) return device[7];
    if(io) n += 128;
    for(uint16_t i = 0; i < sizeof(config1); i += config1[i]){
        if(config1[i] == 7 and config1[i+2] == n){
            return config1[i+5]<<8 bitor config1[i+4];
        }
    }
    return 0; //not found
}
