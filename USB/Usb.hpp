#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>

struct Usb {

    //U1OTGIR
    enum FLAGSOTG : uint8_t {
        //U1OTGIR, U1OTGIE, U1OTGSTAT
        ID = 1<<7,
        MSTIMER = 1<<6, /*no STAT*/
        LINE_STABLE = 1<<5,
        ACTIVITY = 1<<4, /*no STAT*/
        SESSION = 1<<3, BVBUS = 1<<2, AVBUS = 1<<0
    };
    static uint8_t  otg_flags           ();                 //get all
    static bool     otg_flag            (FLAGSOTG);         //get one
    static void     otg_flags_clr       ();                 //clear all
    static void     otg_flags_clr       (uint8_t);          //clear one or more
    static void     otg_flag_clr        (FLAGSOTG);         //clear one

    //U1OTGIE
    static uint8_t  otg_irqs            ();                 //get all
    static bool     otg_irq             (FLAGSOTG);         //get one
    static void     otg_irqs_clr        ();                 //clear all
    static void     otg_irqs_clr        (uint8_t);          //clr one or more
    static void     otg_irq_clr         (FLAGSOTG);         //clr one

    //U1OTGSTAT
    static uint8_t  otg_stat            ();                 //get all
    static bool     otg_stat            (FLAGSOTG);         //get one

    //U1OTGCON
    enum OTG : uint8_t {
        DPPULUP = 1<<7, DMPULUP = 1<<6, DPPULWN = 1<<5, DMPULDWN = 1<<4,
        VBUSON = 1<<3, OTGEN = 1<<2, VBUSCHG = 1<<1, VBUSDIS = 1<<0
    };
    static void     otg                 (OTG, bool);        //set one
    static void     otg                 (uint8_t);          //set one or more

    //U1PWRC
    //power(POWER);           -return POWER bit is set
    //power(POWER, bool);     -set/clr specified POWER bit (true=on)
    enum POWER : uint8_t {
        //U1PWRC
        UACTPND = 1<<7, USLPGRD = 1<<4, USBBUSY = 1<<3, USUSPEND = 1<<1,
        USBPWR = 1<<0
    };
    static bool     power               (POWER);            //get
    static void     power               (POWER, bool);      //set

    //U1IR
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
    //U1IR
    static uint8_t          flags       ();             //get all
    static bool             flag        (FLAGS);        //get one
    static void             flags_clr   (uint8_t);      //clear one or more

    //U1IE
    static uint8_t          irqs        ();             //get all
    static bool             irq         (FLAGS);        //get one
    static void             irqs        (uint8_t);      //set value (all)
    static void             irq         (FLAGS, bool);  //irq on/off

    //U1EIR
    enum EFLAGS : uint8_t {
        //U1EIR, U1EIE
        BSTUFF = 1<<7, BMATRIX = 1<<6, DMA = 1<<5, BTMOUT = 1<<4,
        DATSIZ = 1<<3, CRC16 = 1<<2, CRC5 = 1<<1, EOFR = 1<<1, PID = 1<<0,
        ALLEFLAGS = 255
    };
    //U1EIR
    static uint8_t          eflags      ();             //get all
    static bool             eflag       (EFLAGS);       //get one
    static void             eflags_clr  (uint8_t);      //clear one or more

    //U1EIE
    static uint8_t          eirqs       ();             //get all
    static bool             eirq        (EFLAGS);       //get one
    static void             eirqs       (uint8_t);      //set value (all)
    static void             eirq        (EFLAGS, bool); //eirq on/off

    //U1STAT
    //only valid when TOKEN flag set
    static uint8_t          stat_endp   ();             //endpoint 0-15
    static bool             stat_dir    ();             //0=rx, 1=tx
    static bool             stat_ppbi   ();             //0=even, 1=odd

    //U1CON
    enum CONTROL : uint8_t {
        JSTATE = 1<<7, SE0 = 1<<6, PKTDIS = 1<<5, TOKBUSY = 1<<5,
        USBRST = 1<<4, HOSTEN = 1<<3, RESUME = 1<<2, PPBRST = 1<<1,
        USBEN = 1<<0, SOFEN = 1<<0
    };
    static bool             control     (CONTROL);      //get one
    static void             control     (CONTROL, bool);//set one
    static void             control     (uint8_t);      //set one or more

    //U1ADDR
    static void             low_speed   (bool);         //low speed
    static uint8_t          dev_addr    ();             //get (0-127)
    static void             dev_addr    (uint8_t);      //set

    //U1FRML/U1FRMH
    static uint16_t         frame       ();             //get frame number

    //U1TOK (host)
    enum TOKPID : uint8_t { SETUP = 13, IN = 9, OUT = 1 };
    static void             tok_pid     (uint8_t);
    static void             tok_ep      (uint8_t);

    //U1SOF (host)
    enum SOFVALS : uint8_t {
        SOF64 = 74, SOF32 = 42, SOF16 = 26, SOF8 = 18
    };
    static void             sof_cnt     (SOFVALS);

    //U1BDTP1,2,3
    static void             bdt_addr    (uint32_t);     //set bdt table address
    static uint32_t         bdt_addr    ();             //get bdt table address

    //U1CNFG1
    enum CONFIG : uint8_t {
        EYETEST = 1<<7, OEMON = 1<<6, SIDLE = 1<<4,
        LSDEV = 1<<3, AUTOSUSP = 1<<0
    };
    static void             config      (CONFIG, bool); //set 1bit
    static bool             config      (CONFIG);       //get 1bit
    static void             config      (uint8_t);      //set reg val

    //U1EP0-15
    //endpoint control register bits
    enum EPCTRL {
        LSPD = 1<<7,            /*HOST mode and U1EP0 only*/
        RETRYDIS = 1<<6,        /*HOST mode and U1EP0 only*/
        EPCONDIS = 1<<4,        /*only when TXEN=1 and RXEN=1*/
        EPRXEN = 1<<3, EPTXEN = 1<<2, EPSTALL = 1<<1, EPHSHK = 1<<0
    };
    static void             epcontrol   (uint8_t, EPCTRL, bool);    //set/clr 1bit
    static bool             epcontrol   (uint8_t, EPCTRL);          //get 1bit
    static void             epcontrol   (uint8_t, uint8_t);         //set byte
    static uint8_t          epcontrol   (uint8_t);                  //get byte

    //misc


    //init- return true if vbus present, false if not
    static bool             init        ();

};






