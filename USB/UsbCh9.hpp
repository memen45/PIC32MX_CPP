#pragma once

//USB Chapter 9 stuff

#include <cstdint>

struct UsbCh9 {

    //DeviceDescriptor
    //bDescriptorType
    enum DESCRIPTOR_TYPE {
        DEVICE = 1,
        CONFIGURATION,
        STRING,
        INTERFACE,
        ENDPOINT,
        DEVICE_QUALIFIER,
        OTHER_SPEED,
        INTERFACE_POWER,
        OTG //9
    };

    //ConfigurationDescriptor
    //bmAttributes
    enum CONFIGURATION_ATTRIBUTES {
        REQUIRED = 0x80,
        SELFPOWER = REQUIRED|0x40,
        REMOTEWAKE = REQUIRED|0x20
    };

    //EndpointDescriptor
    //bEndpointAddress
    enum ENDPOINT_ADDRESS {
        OUT0 = 0, OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7,
        OUT8, OUT9, OUT10, OUT11, OUT12, OUT13, OUT14, OUT15,
        IN0 = 128, IN1, IN2, IN3, IN4, IN5, IN6, IN7,
        IN8, IN9, IN10, IN11, IN12, IN13, IN14, IN15,
    };
    //bmAttributes
    enum ENDPOINT_ATTRIBUTES {
        CONTROL = 0, ISOCHRONOUS, BULK, INTERRUPT,
        ISONOSYNC = 0, ISOASYNC = 1<<2, ISOADAPT = 2<<2, ISOSYNC = 3<<2,
        ISODATA = 0, ISOFEEDBACK = 1<<4, ISOIMPFEEDBACK = 2<<4
    };
    //wMaxPacketSize
    enum ENDPOINT_PACKETSIZE {
        INT_LSMAX = 8, INT_FSMAX = 64,
        ISO_FSMAX = 1023,
        BULK_FSMAX = 64, BULK_FSLARGE = 32,
        BULK_FSMEDIUM = 16, BULK_FSSMALL = 8
    };

    //setup packet
    using SetupPkt_t = union {
        uint8_t     packet[8];
        uint16_t    wRequest;
        struct {
            uint8_t     bmRequestType;
            uint8_t     bRequest;
            uint16_t    wValue;
            uint16_t    wIndex;
            uint16_t    wLength;
        };
    };

    //SetupPkt_t.wRequest  (bRequest<<8|bmRequestType)
    enum SETUP_WREQUEST {
        DEV_GET_STATUS = 0x0080, DEV_CLEAR_FEATURE = 0x0100,
        DEV_SET_FEATURE = 0x0300, DEV_SET_ADDRESS = 0x0500,
        DEV_GET_DESCRIPTOR = 0x0680, DEV_SET_DESCRIPTOR = 0x0700,
        DEV_GET_CONFIGURATION = 0x0880, DEV_SET_CONFIGURATION = 0x0900,

        IF_GET_STATUS = 0x0081, IF_CLEAR_FEATURE = 0x0101,
        IF_SET_FEATURE = 0x0301, IF_GET_IFACE = 0x0A81,
        IF_SET_IFACE = 0x1101,

        EP_GET_STATUS = 0x0082, EP_CLEAR_FEATURE = 0x0102,
        EP_SET_FEATURE = 0x0302, EP_SYNC_HFRAME = 0x1202,
    };
    //SetupPkt_t.bRequest
    enum SETUP_BREQUEST {
        GET_STATUS = 0x00, CLEAR_FEATURE = 0x01,
        SET_FEATURE = 0x03, SET_ADDRESS = 0x05,
        GET_DESCRIPTOR = 0x06, SET_DESCRIPTOR = 0x07,
        GET_CONFIGURATION = 0x08, SET_CONFIGURATION = 0x09,

        GET_IFACE = 0x0A,
        SET_IFACE = 0x11,

        SYNC_HFRAME = 0x12
    };

    enum TOKEN_TYPE {
        OUT = 1, IN = 9, SETUP = 13
    };

};

