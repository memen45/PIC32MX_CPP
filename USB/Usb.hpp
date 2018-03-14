#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "Reg.hpp"
#include "UsbCh9.hpp"
#include "Pins.hpp"
#include "Irq.hpp"


#include "UsbBuf.hpp"


struct Usb {

    /*
     flags, interrupts
        FLAGS enum contains all flag/irq names

        flags();                -return val of all flags (uint8_t)
        flag(FLAGS);            -get specified flag
        flags_clr(uint8_t);     -clear flag(s) as bitmask (1=clear)

        irqs();                 -return val of all irqs (uin8_t)
        irq(FLAGS);             -get specified irq
        irqs(uint8t);           -set value (all)
        irq(FLAGS, bool);       -one irq on/off (true=on)

        eflag, eirq functions mirror above but are for error flags
    */

    enum FLAGS : uint8_t {
        //U1IR, U1IE
        STALL = 1<<7, ATTACH = 1<<6 /*host only*/, RESUME = 1<<5, IDLE = 1<<4,
        TOKEN = 1<<3, SOF = 1<<2, ERROR = 1<<1, RESET = 1<<0,
        DETACH = 1<<0 /*host only*/,  ALLFLAGS = 255
    };

    using flags_t = union {
        struct {
        unsigned reset  :1;
        unsigned error  :1;
        unsigned sof    :1;
        unsigned token  :1;
        unsigned idle   :1;
        unsigned resume :1;
        unsigned attach :1;
        unsigned stall  :1;
        };
        uint8_t all;
    };

    enum EFLAGS : uint8_t {
        //U1EIR, U1EIE
        BSTUFF = 1<<7, BMATRIX = 1<<6, DMA = 1<<5, BTMOUT = 1<<4,
        DATSIZ = 1<<3, CRC16 = 1<<2, CRC5 = 1<<1, EOFR = 1<<1, PID = 1<<0,
        ALLEFLAGS = 255
    };

    using eflags_t = union {
        struct {
        unsigned pid    :1;
        unsigned crc5   :1;
        unsigned crc16  :1;
        unsigned datsiz :1;
        unsigned btmout :1;
        unsigned dma    :1;
        unsigned bmatrix:1;
        unsigned bstuff :1;
        };
        uint8_t all;
    };

    static uint8_t          flags       ();             //get all
    static bool             flag        (FLAGS);        //get one
    static void             flags_clr   (uint8_t);      //clear one or more

    static uint8_t          irqs        ();             //get all
    static bool             irq         (FLAGS);        //get one
    static void             irqs        (uint8_t);      //set value (all)
    static void             irq         (FLAGS, bool);  //irq on/off

    static uint8_t          eflags      ();             //get all
    static bool             eflag       (EFLAGS);       //get one
    static void             eflags_clr  (uint8_t);      //clear one or more

    static uint8_t          eirqs       ();             //get all
    static bool             eirq        (EFLAGS);       //get one
    static void             eirqs       (uint8_t);      //set value (all)
    static void             eirq        (EFLAGS, bool); //eirq on/off

    //power(POWER);           -return POWER bit is set
    //power(POWER, bool);     -set/clr specified POWER bit (true=on)
    enum POWER : uint8_t {
        //U1PWRC
        PENDING = 1<<7, SLEEPGUARD = 1<<4, BUSY = 1<<3, SUSPEND = 1<<1,
        USBPWR = 1<<0
    };
    static bool             power               (POWER);
    static void             power               (POWER, bool);

    //only valid when TOKEN flag set
    using stat_t = union {
        struct {
            unsigned        :2;
            unsigned eveodd :1;
            unsigned trx    :1;
            unsigned endpt  :4;
        };
        struct {
            unsigned        :2;
            unsigned bdidx  :2;
            unsigned        :4;
        };
        struct {
            unsigned        :2;
            unsigned bdn    :6;
        };
        uint8_t all;
    };

    static uint8_t          stat                ();

    enum CONTROL : uint8_t {
        JSTATE = 1<<7, SE0 = 1<<6, PKTDIS = 1<<5, TOKBUSY = 1<<5,
        USBRESET = 1<<4, HOSTEN = 1<<3, RESUMEEN = 1<<2, PPRESET = 1<<1,
        USBEN = 1<<0, SOFEN = 1<<0
    };

    static bool             control             (CONTROL);
    static void             control             (CONTROL, bool);
    static void             control             (uint8_t);

    //usb device address get/set
    static uint8_t          dev_addr            ();
    static void             dev_addr            (uint8_t);

    //get frame number
    static uint16_t         frame               ();

    //get/set bdt table address
    static void             bdt_addr            (uint32_t);
    static uint32_t         bdt_addr            ();

    //get/set config register
    enum CONFIG : uint8_t {
        EYETEST = 1<<7, OEMON = 1<<6, SIDLE = 1<<4,
        LSDEV = 1<<3, AUTOSUSP = 1<<0
    };

    static void             config              (CONFIG, bool); //set 1bit
    static bool             config              (CONFIG);       //get 1bit
    static void             config              (uint8_t);      //set reg val

    //usb bus state
    enum STATE {
        DETACHED,   //usb peripheral shut down (I made this state up)
        ATTACHED,   //usb is init, but not powered
        POWERED,    //vbus now has power, but no reset seen yet
        DEFAULT,    //reset received, device address is 0 (default)
        ADDRESS,    //now have unique device address
        CONFIGURED, //now configured (and not in suspended state)
        SUSPENDED   //no activity for >3ms
    };

    static STATE state;

    //handlers
    static void     init                ();
    static void     detach              (void);
    static void     attach              (void);

};






