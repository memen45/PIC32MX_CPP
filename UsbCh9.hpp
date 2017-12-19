#pragma once

/*=============================================================================
 USB Chapter 9 stuff - imcomplete
=============================================================================*/

#include <cstdint>

class UsbCh9 {

    public:

    //============================================================
    // Descriptors
    //============================================================

    //bDescriptorType
    typedef enum {
        DEVICE = 1, CONFIG, STRING, IFACE,
        ENDP, QUAL, OTHER, IFACEPWR, OTG
    } bDescriptorType_t;



    //Device Descriptor
    typedef struct __attribute__ ((packed)) {
        uint8_t bLength;            // 18
        uint8_t bDescriptorType;    // 1 - DEVICE
        uint16_t bcdUSB;
        uint8_t bDeviceClass;
        uint8_t bDeviceSubClass;
        uint8_t bDeviceProtocol;
        uint8_t bMaxPacketSize0;    // 8,16,32,64
        uint16_t idVendor;
        uint16_t idProduct;
        uint16_t bcdDevice;
        uint8_t iManufacturer;
        uint8_t iProduct;
        uint8_t iSerialNumber;
        uint8_t bNumConfigurations;
    } DeviceDescriptor_t;



    //Configuration Descriptor Structure
    typedef struct __attribute__ ((packed)) {
        uint8_t bLength;            // 9
        uint8_t bDescriptorType;    // 2 - CONFIG
        uint16_t wTotalLength;
        uint8_t bNumInterfaces;
        uint8_t bConfigurationValue;
        uint8_t iConfiguration;
        uint8_t bmAttributes;
        uint8_t bMaxPower;          // in 2ma units
    } ConfigurationDescriptor_t;

    typedef enum {
        REQUIRED = 0x80, SELFPWR = 0xC0, REMWAKE = 0xA0
    } bmAttributesCONFIG_t;



    //Interface Descriptor
    typedef struct __attribute__ ((packed)){
        uint8_t bLength;            // 9
        uint8_t bDescriptorType;    // 4 - IFACE
        uint8_t bInterfaceNumber;
        uint8_t bAlternateSetting;
        uint8_t bNumEndpoints;
        uint8_t bInterfaceClass;
        uint8_t bInterfaceSubClass;
        uint8_t bInterfaceProtocol;
        uint8_t iInterface;
    } InterfaceDescriptor_t;



    //Endpoint Descriptor
    typedef struct __attribute__ ((packed)){
        uint8_t bLength;            // 7
        uint8_t bDescriptorType;    // 5 - ENDP
        uint8_t bEndpointAddress;   // OUT0-OUT15, IN0-IN15
        uint8_t bmAttributes;
        uint16_t wMaxPacketSize;
        uint8_t bInterval;
    } EndpointDescriptor_t;

    typedef enum {
        OUT0 = 0, OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7,
        OUT8 = 8, OUT9, OUT10, OUT11, OUT12, OUT13, OUT14, OUT15,
        IN0 = 128, IN1, IN2, IN3, IN4, IN5, IN6, IN7,
        IN8 = 136, IN9, IN10, IN11, IN12, IN13, IN14, IN15,
    } bEndpointAddressENDP_t;

    typedef enum {
        CONTROL = 0, ISO, BULK, INT,
        ISONOSYNC = 0, ISOASYNC = 1<<2, ISOADAPT = 2<<2, ISOSYNC = 3<<2,
        ISODATA = 0, ISOFEED = 1<<4, ISOEFEED = 2<<4
    } bmAttributesENDP_t;

    typedef enum {
        LSINTMAX = 8, FSINTMAX = 64,
        FSISOMAX = 1023, FSBULKMAX = 64,
        FSBULKLG = 32, FSBULKMED = 16, FSBULKSM = 8
    } wMaxPacketSizeENDP_t;



    //String descriptor (header only)
    typedef struct __attribute__ ((packed)){
        uint8_t bLength;
        uint8_t bDescriptorType;    // 3 - STRING
    } StringDescriptor_t;




    //============================================================
    // Setup Packet
    //============================================================

    //setup packet
    typedef union __attribute__ ((packed)){
        uint8_t packet[8];
        uint16_t wRequest;
        struct {
            uint8_t bmRequestType;
            uint8_t bRequest;
            uint16_t wValue;
            uint16_t wIndex;
            uint16_t wLength;
        };
    } SetupPacket_t;

    typedef enum  {
        DAT2DEV = 0<<7, DAT2HOST = 1<<7,
        TYPSTD = 0<<5, TYPCLASS = 1<<5, TYPVEND = 2<<5,
        FORDEV = 0, FORIFACE = 1, FORENDP = 2, FOROTHER = 3
    } bmRequestTypeSETUP_t;

    typedef enum { //uint16_t wRequest; (bRequest<<8|bmRequestType)
        DEV_GET_STATUS = 0x0080, DEV_CLEAR_FEATURE = 0x0100,
        DEV_SET_FEATURE = 0x0300, DEV_SET_ADDRESS = 0x0500,
        DEV_GET_DESCRIPTOR = 0x0680, DEV_SET_DESCRIPTOR = 0x0700,
        DEV_GET_CONFIGURATION = 0x0880, DEV_SET_CONFIGURATION = 0x0900,

        IF_GET_STATUS = 0x0081, IF_CLEAR_FEATURE = 0x0101,
        IF_SET_FEATURE = 0x0301, IF_GET_IFACE = 0x0A81,
        IF_SET_IFACE = 0x1101,

        EP_GET_STATUS = 0x0082, EP_CLEAR_FEATURE = 0x0102,
        EP_SET_FEATURE = 0x0302, EP_SYNC_HFRAME = 0x1202
    } bRequestStdSETUP_t;



    //============================================================
    // USB Protocol -
    // |FRAME|FRAME|FRAME|...
    //    ^
    //    SOF|TRANSACTION|TRANSACTION|TRANSACTION|...
    //             ^
    //             TOKEN|[DATA]|HANDSHAKE
    //
    //  Transaction-
    //  Token packet, [optional Data packet], Status packet
    //
    //  all packets-
    //      Sync    8bits (LS & FS)
    //      PID     4bits pid, 4bits npid
    //      ADDR    7bits (0-127)
    //      ENDP    4bits (LS only 2bits used)
    //      CRC     5bits for Token, 16bits for Data
    //      EOP     SE0 for 2bit times, J 1 bit time
    //============================================================

    typedef enum {
        OUT = 1, IN = 9, SOF = 5, SETUP = 13,         //Token
        DATA0 = 3, DATA1 = 11, DATA2 = 7, MDATA = 15, //Data
        ACK = 2, NAK = 10, STALL = 14, NYET = 6,      //Handshake
        PRE = 12, ERR = 12, SPLIT = 8, PING = 4       //Special
    } Pid_t;

    //============================================================
    // Token Packet - IN, OUT, SETUP
    //============================================================
    // Sync PID	ADDR ENDP CRC5 EOP

    //============================================================
    // Token Packet - SOF
    //============================================================
    //Sync PID FrameNumber CRC5 EOP
    //11bit Frame Number every 1ms for FS, every 125us for HS

    //============================================================
    // Data Packet - DATA0, DATA1 (HS DATA2, MDATA)
    //============================================================
    //Sync PID Data CRC16 EOP
    //LS max 8bytes, FS max 1023bytes, HS max 1024bytes

    //============================================================
    // Handshake Packet - ACK, NAK, STALL
    //============================================================
    //Sync PID EOP




};
