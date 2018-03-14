#include "UsbDescriptors.hpp"
#include "UsbCh9.hpp"
#include "UsbBuf.hpp"


//CDC Demo
const UsbCh9::DeviceDescriptor_t device = {
    18,             //length
    UsbCh9::DEVICE, //1
    0x0200,         //(bcd) usb 2.0
    2,              //class (CDC device)
    0,              //subclass
    0,              //protocol
    64,             //max ep0 packet size
    0x04D8,         //VID
    0x000A,         //PID
    0x0100,         //(bcd) release number
    1,              //string index- manufacturer
    2,              // -product
    0,              // -serial number
    1               //#of configurations
};



const uint8_t config1[] = {
    //let function fill in size (third,fourth byte, word size)
    9, UsbCh9::CONFIGURATION, 0, 0, 2, 1, 0, UsbCh9::SELFPOWER, 50,
    //interface0
    9, UsbCh9::INTERFACE, 0, 0, 0, 2, 2, 1, 0,
    //cdc descriptors
    5, 0x24, 0, 0x10, 0x01,     //cdc header
    4, 0x24, 2, 2,              //acm, the last 2 is support line requests only
    5, 0x24, 6, 0, 1,           //union comm id=0, data id=1
    5, 0x24, 1, 0, 1,            //call management
    //endpoint  //8,0 = 0x0008
    7, UsbCh9::ENDPOINT, UsbCh9::IN1, UsbCh9::INTERRUPT, 8, 0, 2,
    //interface1
    9, UsbCh9::INTERFACE, 1, 0, 2, 10, 0, 0, 0,
    //endpoint //64,0 = 0x0040
    7, UsbCh9::ENDPOINT, UsbCh9::OUT2, UsbCh9::BULK, 64, 0, 0,
    //endpoint
    7, UsbCh9::ENDPOINT, UsbCh9::IN2, UsbCh9::BULK, 64, 0, 0
};


//Language code string descriptor
const char* strings[] = {
    "\x09\x04",                     //language (low byte, high byte)
    "Microchip Techonology Inc.",   //manufacturer
    "CDC RS-232 Emulation Demo"     //product
};





void get_device(UsbBuf::buffer64_t& buf){
    if(buf.buf_size < device.bLength) return;
    const uint8_t* dev = (const uint8_t*)&device;
    for(; buf.status < device.bLength; buf.status++){
        buf.buf[buf.status] = *dev++;
    }
}

void get_config(UsbBuf::buffer64_t& buf, uint8_t idx){
    if(idx != 1 or buf.buf_size < sizeof(config1)) return;
    uint8_t* cfg = (uint8_t*)config1;
    for(;buf.status < sizeof(config1); buf.status++){
        buf.buf[buf.status] = *cfg++;
    }
    buf.buf[2] = buf.status; //fill in total config size
    buf.buf[3] = buf.status>>8;
}

void get_string(UsbBuf::buffer64_t& buf, uint8_t idx){
    if(idx > sizeof(strings)) return;
    const char* str = strings[idx];
    buf.buf[1] = UsbCh9::STRING; //type
    uint8_t i = 0;
    for(; *str && i < buf.buf_size-1; i += 2){
        buf.buf[i] = *str++;
        buf.buf[i+1] = 0; //wide char
    }
    buf.buf[0] = i; //fill in string length
    buf.status = *str ? 0 : i; //if string too long, return 0
}

//m_setup_pkt.wValue high=descriptor type, low=index
//device=1, config=2, string=3
void UsbDescriptors::get(uint16_t wValue, UsbBuf::buffer64_t& buf){
    uint8_t idx = wValue;
    uint8_t typ = wValue>>8;
    buf.status = 0; //assume failure
    if(typ == 1){ get_device(buf); }
    if(typ == 2){ get_config(buf, idx); }
    if(typ == 3){ get_string(buf, idx); }
}