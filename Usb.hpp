#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "Pins.hpp"

struct Usb {

    //BDT table

    //set/change as needed- instead of fixing bdt table size
    //exactly to configuration used, just set max endpoint here
    //and change if needed- (there will be some wasted space)
    //will assume most configs will use ep <= 7
    static const uint8_t max_endpoint = 7;

    //USB Buffer Descriptor Control Format
    using ctrl_t = union {
        struct {
        unsigned        :2;
        unsigned bstall :1;
        unsigned dts    :1;
        unsigned ninc   :1;
        unsigned keep   :1;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        };
        uint8_t val8;
        uint32_t val32;
    };
    //USB Buffer Descriptor Status Format
    using stat_t = union {
        struct {
        unsigned        :2;
        unsigned pid    :4;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        };
        uint8_t val8;
        uint32_t val32;
    };

    using bdt_t = struct {
        union { ctrl_t ctrl; stat_t stat; };
        uint32_t bufaddr;
    };

    static volatile bdt_t bdt_table[max_endpoint+1][2][2]
        __attribute__ ((aligned (512)));

        static auto
    bdt_init() -> void;

    //vbus pin
    static const Pins::RPN vbus_pin_n = Pins::B6;

        static auto
    vbus_ison() -> bool;

    //irq priority
    static const uint8_t usb_irq_pri = 1;       //usb interrupt priority
    static const uint8_t usb_irq_subpri = 0;    //usb interrupt sub-priority


    //U1OTGIR/IE, U1EIR/IE, U1IR/IE
    //all flags combined into uint32_t
    //U1OTGIR to byte2, U1EIR to byte1, U1IR to byte0
    /*
     flags, interrupts
        FLAGS enum contains all flag/irq names

        flags();                -return val of all flags (uint32_t)
        flag(FLAGS);            -get specified flag
        flags_clr(uint32_t);    -clear flag(s) as bitmask (1=clear)

        irqs();                 -return val of all irqs (uin32_t)
        irq(FLAGS);             -get specified irq
        irqs(uint32_t);         -set value of all irq enable
        irq(FLAGS, bool);       -enable/disable one irq (true=on)

        eflag, eirq functions mirror above but are for error flags
    */

        enum
    FLAGS : uint32_t {
        //U1OTGIR, U1OTGIE, U1OTGSTAT
        ID = 1<<23,
        T1MSEC = 1<<22,     /*no STAT*/
        LSTATE = 1<<21,
        ACTV = 1<<20,       /*no STAT*/
        SESVD = 1<<19,
        SESEND = 1<<18,
        //unused
        VBUSVD = 1<<16,
        ALLOTGIF = (ID|T1MSEC|LSTATE|ACTV|SESVD|SESEND),

        //U1EIR, U1EIE
        BTS = 1<<15,
        BMX = 1<<14,
        DMA = 1<<13,
        BTO = 1<<12,
        DFN8 = 1<<11,
        CRC16 = 1<<10,
        CRC5 = 1<<9,
        UEOF = 1<<9,         /*host only*/
        PID = 1<<8,
        ALLEIF = (BTS|BMX|DMA|BTO|DFN8|CRC16|CRC5|PID),

        //U1IR, U1IE
        STALL = 1<<7,
        ATTACH = 1<<6,      /*host only*/
        RESUME = 1<<5,
        IDLE = 1<<4,
        TRN = 1<<3,
        SOF = 1<<2,
        UERR = 1<<1,
        URST = 1<<0,
        DETACH = 1<<0,      /*host only*/
        ALLIF = (STALL|ATTACH|RESUME|IDLE|TRN|SOF|UERR|URST)
    };

        static auto
    flags () -> uint32_t;               //get all

        static auto
    flag (FLAGS) -> bool;               //get one

        static auto
    flags_clr (uint32_t) -> void;       //clear one or more

        static auto
    irqs () -> uint32_t;                //get all

        static auto
    irq (FLAGS) -> bool;                //get one

        static auto
    irqs (uint32_t) -> void;            //set value (all)

        static auto
    irq (FLAGS, bool) -> void;          //irq on/off

    //U1OTGSTAT

        static auto
    otg_stat () -> uint32_t;            //get all

        static auto
    otg_stat (FLAGS) -> bool;           //get one

    //U1OTGCON

        enum
    OTG : uint8_t {
        DPPULUP = 1<<7, DMPULUP = 1<<6, DPPULWN = 1<<5, DMPULDWN = 1<<4,
        VBUSON = 1<<3, OTGEN = 1<<2, VBUSCHG = 1<<1, VBUSDIS = 1<<0
    };
        static auto
    otg (OTG, bool) -> void;            //set one

        static auto
    otg (uint8_t) -> void;              //set one or more

    //U1PWRC

        enum
    POWER : uint8_t {
        //U1PWRC
        UACTPND = 1<<7, USLPGRD = 1<<4, USBBUSY = 1<<3, USUSPEND = 1<<1,
        USBPWR = 1<<0
    };
        static auto
    power (POWER) -> bool;              //get

        static auto
    power (POWER, bool) -> void;        //set/clr

    //U1STAT

    //only valid when TOKEN flag set
    //value return is shifted right 2 bits, so can be used as bdt index
        static auto
    stat () -> uint8_t;                 //U1STAT>>2

    //U1CON

        enum
    CONTROL : uint8_t {
        JSTATE = 1<<7, SE0 = 1<<6, PKTDIS = 1<<5, TOKBUSY = 1<<5,
        USBRST = 1<<4, HOSTEN = 1<<3, RESUM = 1<<2, PPBRST = 1<<1,
        USBEN = 1<<0, SOFEN = 1<<0
    };
        static auto
    control (CONTROL) -> bool;          //get one

        static auto
    control (CONTROL, bool) -> void;    //set one

        static auto
    control (uint8_t) -> void;          //set one or more

    //U1ADDR

        static auto
    low_speed (bool) -> void;           //low speed

        static auto
    dev_addr () -> uint8_t;             //get (0-127)

        static auto
    dev_addr (uint8_t) -> void;         //set

    //U1FRML/U1FRMH

        static auto
    frame () -> uint16_t;               //get frame number

    //U1TOK (host)

        enum
    TOKPID : uint8_t { SETUP = 13<<4, IN = 9<<4, OUT = 1<<4 };
        static auto
    tok_pid (TOKPID) -> void;

        static auto
    tok_ep (uint8_t) -> void;

    //U1SOF (host)

        enum
    SOFVALS : uint8_t {
        SOF64 = 74, SOF32 = 42, SOF16 = 26, SOF8 = 18
    };
        static auto
    sof_cnt (SOFVALS) -> void;

    //U1BDTP1,2,3

        static auto
    bdt_addr (uint32_t) -> void;        //set bdt table address

        static auto
    bdt_addr (uint8_t = 0, bool = 0, bool = 0) -> volatile bdt_t*;
                                        //bdt table address (ep#,tx/rx,even/odd)
    //U1CNFG1

        enum
    CONFIG : uint8_t {
        EYETEST = 1<<7, OEMON = 1<<6, SIDLE = 1<<4,
        LSDEV = 1<<3, AUTOSUSP = 1<<0
    };
        static auto
    config (CONFIG, bool) -> void;      //set 1bit

        static auto
    config (CONFIG) -> bool;            //get 1bit

        static auto
    config (uint8_t) -> void;           //set reg val

    //U1EP0-15

        enum
    EPCTRL {
        LSPD = 1<<7,            /*HOST mode and U1EP0 only*/
        RETRYDIS = 1<<6,        /*HOST mode and U1EP0 only*/
        EPCONDIS = 1<<4,        /*only when TXEN=1 and RXEN=1*/
        EPRXEN = 1<<3, EPTXEN = 1<<2, EPSTALL = 1<<1, EPHSHK = 1<<0
    };
        static auto
    epcontrol (uint8_t, EPCTRL, bool) -> void;  //set/clr 1bit

        static auto
    epcontrol (uint8_t, EPCTRL) -> bool;        //get 1bit

        static auto
    epcontrol (uint8_t, uint8_t) -> void;       //set byte

        static auto
    epcontrol (uint8_t) -> uint8_t;             //get byte

    //misc

        static auto
    reset () -> void;     //all writable regs to reset val

};
