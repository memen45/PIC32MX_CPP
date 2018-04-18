#pragma once

//USB Chapter 9 stuff

#include <cstdint>

struct UsbCh9 {

    //DeviceDescriptor_t.bDescriptorType
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

    //ConfigurationDescriptor_t.bmAttributes
    enum CONFIGURATION_ATTRIBUTES {
        REQUIRED = 0x80,
        SELFPOWER = REQUIRED|0x40,
        REMOTEWAKE = REQUIRED|0x20
    };

    //EndpointDescriptor_t.bEndpointAddress
    enum ENDPOINT_ADDRESS {
        OUT0 = 0, OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7,
        OUT8, OUT9, OUT10, OUT11, OUT12, OUT13, OUT14, OUT15,
        IN0 = 128, IN1, IN2, IN3, IN4, IN5, IN6, IN7,
        IN8, IN9, IN10, IN11, IN12, IN13, IN14, IN15,
    };
    //EndpointDescriptor_t.bmAttributes
    enum ENDPOINT_ATTRIBUTES {
        CONTROL = 0, ISOCHRONOUS, BULK, INTERRUPT,
        ISONOSYNC = 0, ISOASYNC = 1<<2, ISOADAPT = 2<<2, ISOSYNC = 3<<2,
        ISODATA = 0, ISOFEEDBACK = 1<<4, ISOIMPFEEDBACK = 2<<4
    };
    //EndpointDescriptor_t.wMaxPacketSize
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

    //we will put cdc stuff here for now
    //SetupPkt_t.wRequest  (bRequest<<8|bmRequestType)
    enum SETUP_WREQUEST_CDC {
        CDC_SEND_ENCAP_COMMAND = 0x0021,
        CDC_GET_ENCAP_RESPONSE = 0x0121,
        CDC_SET_COMM_FEATURE = 0x0221,
        CDC_GET_COMM_FEATURE = 0x0321,
        CDC_CLEAR_COMM_FEATURE = 0x0421,
        CDC_SET_LINE_CODING = 0x2021,
        CDC_GET_LINE_CODING = 0x2121,
        CDC_SET_CONTROL_LINE_STATE = 0x2221,
        CDC_SEND_BREAK = 0x2321
    };

    enum TOKEN_TYPE {
        OUT = 1, IN = 9, SETUP = 13
    };

};

/*

============================================================
 HS stuff not for us - but may be referenced below

 USB Protocol -
 |FRAME|FRAME|FRAME|...
    ^
    SOF|TRANSACTION|TRANSACTION|TRANSACTION|...
             ^
             TOKEN|[DATA]|HANDSHAKE

  Transaction-
  Token packet, [optional Data packet], Handshake packet

============================================================


============================================================
 Token Packet - IN(pid 9), OUT(pid 1), SETUP(pid 13)
============================================================
 Sync PID ADDR ENDP CRC5 EOP

  packets in/out/setup-
      Sync    8bits (LS & FS)
      ................................................
      PID     4bits pid, 4bits npid
      ADDR    7bits (0-127)
      ENDP    4bits (LS only 2bits used)
      CRC     5bits
      ................................................
      EOP     SE0 for 2bit times, J 1 bit time

============================================================
 Token Packet - SOF(pid 5)
 every 1ms for FS, every 125us for HS
============================================================
Sync PID FrameNumber CRC5 EOP

      Sync    8bits (LS & FS)
      ................................................
      PID     4bits pid, 4bits npid
      FRAME   11bits (0-2047)
      CRC     5bits
      ................................................
      EOP     SE0 for 2bit times, J 1 bit time

============================================================
 Data Packet - DATA0(pid 3), DATA1(11) (HS DATA2, MDATA)
============================================================
    Sync PID Data CRC16 EOP

    Sync    8bits (LS & FS)
    ................................................
    PID     4bits pid, 4bits npid
    DATA    data bytes (0- 8 LS, 1023 FS)
    CRC     16bits
    ................................................
    EOP     SE0 for 2bit times, J 1 bit time

============================================================
 Handshake Packet - ACK(pid 2), NAK(pid 10), STALL(pid 14)
============================================================
    Sync PID EOP

    Sync    8bits (LS & FS)
    ................................................
    PID     4bits pid, 4bits npid
    ................................................
    EOP     SE0 for 2bit times, J 1 bit time


============================================================
    PID decoding- 4bits 0b0000 - 0b1111
============================================================

    0bxx00 = Special
         0b0000 unused
         0b0100 PING(HS)     4
         0b1000 SPLIT(HS)    8
         0b1100 PRE,ERR(HS)  12
    0bxx01 = Token
         0b0001 OUT          1
         0b0101 SOF          5
         0b1001 IN           9
         0b1101 SETUP        13
    0bxx10 = Handshake
         0b0010 ACK          2
         0b0110 NYET (HS)    6
         0b1010 NAK          10
         0b1110 STALL        14
    0bxx11 = Data
         0b0011 DATA0        3
         0b0111 DATA2 (HS)   7
         0b1011 DATA1        11
         0b1111 MDATA (HS)   15
============================================================


============================================================
 Control transfer
============================================================
 endpoint 0 must support, others optional
 both in/out of endpoint is used
 2-3 stages of every control transfer- setup,[data],status

 control write transfer, (setup transaction)-
    setup,data(data0),ack (all hardware)
    data=8bytes

 control write transfer, (data transaction)-
    out,data(out,data1,data0,...),ack/nak/stall

 control write transfer, (status transaction)-
    in,data(in,data1-0bytes),ack



 control read transfer, (setup transaction)-
    setup,data(data0),ack (all hardware)
    data=8bytes

 control read transfer, (data transaction)-
    in,data(in,data1,data0,...),ack

 control read transfer, (status transaction)-
    out,data(out,data1-0bytes),ack/nak/stall

 */
