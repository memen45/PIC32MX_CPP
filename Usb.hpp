#pragma once

/*=============================================================================
 USB peripheral - PIC32MM0256GPM064
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "UsbCh9.hpp"
#include "Pins.hpp"
#include "Irq.hpp"



///////////////////////////////////////////////////////////////////////////////
/////// user provided data ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//---power/wakeup options-------------------------------------------------------
static const bool my_self_powered = 1;      //self-powered=1, bus-powered=0
static bool my_remote_wakeup = 1;           //remote wakeup enabled=1
//---endpoints------------------------------------------------------------------
static const uint8_t my_last_endp = 4;      //last endpoint number used
static const uint8_t my_n_endp = 3;         //number of endpoints used
static const uint8_t my_buffer_size = 64;   //size of buffers (all same)
static const uint8_t my_buffer_count = 8;   //number of buffers in buffer pool
//---vbus pin-------------------------------------------------------------------
static Pins vbus_pin(Pins::B6);             //Vbus pin
//---interrupt------------------------------------------------------------------
static const uint8_t usb_irq_pri = 5;       //usb interrupt priority
static const uint8_t usb_irq_subpri = 0;    //usb interrupt sub-priority
//priority will need to be set in Irq::init(), and if using shadow register
//also need to set to this priority using Irq::shadow_set()
//(isr function is in UsbHandler section, although not in any class can be
// anywhere)
ISR(USB, 5, SRS); //declared only, defined later in this file
///////////////////////////////////////////////////////////////////////////////
/////// usb uses irq only, no polling /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



//simple way to separate classes/structs for now
//__inline include______________________________________________________________

#include "UsbBuf.hpp"

//______________________________________________________________________________



struct Usb {
/*______________________________________________________________________________

    Usb hardware functions
    ( UsbOTG:: handles OTG registers )
    ( endpoint registers U1EPn handled by UsbEndpt:: )
______________________________________________________________________________*/

    /*..........................................................................
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
    ..........................................................................*/

    enum FLAGS : uint8_t {
        //U1IR, U1IE
        STALL = 1<<7, ATTACH = 1<<6 /*host only*/, RESUME = 1<<5, IDLE = 1<<4,
        TOKEN = 1<<3, SOF = 1<<2, ERROR = 1<<1, RESET = 1<<0,
        DETACH = 1<<0 /*host only*/,  ALLFLAGS = 255
    };

    typedef union {
        struct {
        unsigned reset:1;
        unsigned error:1;
        unsigned sof:1;
        unsigned token:1;
        unsigned idle:1;
        unsigned resume:1;
        unsigned attach:1;
        unsigned stall:1;
        };
        uint8_t all;
    } flags_t;

    enum EFLAGS : uint8_t {
        //U1EIR, U1EIE
        BSTUFF = 1<<7, BMATRIX = 1<<6, DMA = 1<<5, BTMOUT = 1<<4,
        DATSIZ = 1<<3, CRC16 = 1<<2, CRC5 = 1<<1, EOF = 1<<1, PID = 1<<0,
        ALLEFLAGS = 255
    };

    typedef union {
        struct {
        unsigned pid:1;
        unsigned crc5:1;
        unsigned crc16:1;
        unsigned datsiz:1;
        unsigned btmout:1;
        unsigned dma:1;
        unsigned bmatrix:1;
        unsigned bstuff:1;
        };
        uint8_t all;
    } eflags_t;

    static volatile uint8_t flags       ();             //get all
    static volatile bool    flag        (FLAGS);        //get one
    static void             flags_clr   (uint8_t);      //clear one or more

    static uint8_t          irqs        ();             //get all
    static bool             irq         (FLAGS);        //get one
    static void             irqs        (uint8_t);      //set value (all)
    static void             irq         (FLAGS, bool);  //irq on/off

    static volatile uint8_t eflags      ();             //get all
    static volatile bool    eflag       (EFLAGS);       //get one
    static void             eflags_clr  (uint8_t);      //clear one or more

    static uint8_t          eirqs       ();             //get all
    static bool             eirq        (EFLAGS);       //get one
    static void             eirqs       (uint8_t);      //set value (all)
    static void             eirq        (EFLAGS, bool); //eirq on/off


    /*..........................................................................
     power
        POWER enum contains all power names

        power(POWER);           -return POWER bit is set
        power(POWER, bool);     -set/clr specified POWER bit (true=on)
    ..........................................................................*/

    enum POWER : uint8_t {
        //U1PWRC
        PENDING = 1<<7, SLEEPGUARD = 1<<4, BUSY = 1<<3, SUSPEND = 1<<1,
        USBPWR = 1<<0
    };

    static bool             power               (POWER);
    static void             power               (POWER, bool);

    /*..........................................................................
     stat
        only valid when TOKEN flag set
    ..........................................................................*/
    typedef union {
        struct {
        unsigned :2; unsigned eveodd:1; unsigned trx:1; unsigned endpt:4;
        };
        struct { unsigned :2; unsigned bdidx:2; unsigned :4; };
        struct { unsigned :2; unsigned bdn:6; };
        uint8_t all;
    } stat_t;

    static volatile uint8_t stat                ();


    /*..........................................................................
     control

    ..........................................................................*/
    enum CONTROL : uint8_t {
        JSTATE = 1<<7, SE0 = 1<<6, PKTDIS = 1<<5, TOKBUSY = 1<<5,
        USBRESET = 1<<4, HOSTEN = 1<<3, RESUMEEN = 1<<2, PPRESET = 1<<1,
        USBEN = 1<<0, SOFEN = 1<<0
    };

    static volatile bool    control             (CONTROL);
    static void             control             (CONTROL, bool);
    static void             control             (uint8_t);


    /*..........................................................................
     dev_addr
        usb device address get/set
    ..........................................................................*/
    static uint8_t          dev_addr            ();
    static void             dev_addr            (uint8_t);


    /*..........................................................................
     frame
        get frame number
    ..........................................................................*/
    static uint16_t         frame               ();


    /*..........................................................................
     bdt_addr
        get/set bdt table address
    ..........................................................................*/
    static void             bdt_addr            (uint32_t);
    static uint32_t         bdt_addr            ();


    /*..........................................................................
     config
        get/set config register
    ..........................................................................*/
    enum CONFIG : uint8_t {
        EYETEST = 1<<7, OEMON = 1<<6, SIDLE = 1<<4,
        LSDEV = 1<<3, AUTOSUSP = 1<<0
    };

    static void             config              (CONFIG, bool); //set 1bit
    static bool             config              (CONFIG);       //get 1bit
    static void             config              (uint8_t);      //set reg val



    /*..........................................................................
     state
        usb bus state
    ..........................................................................*/
    typedef enum {
        DETACHED,   //usb peripheral shut down (I made this state up)
        ATTACHED,   //usb is init, but not powered
        POWERED,    //vbus now has power, but no reset seen yet
        DEFAULT,    //reset received, device address is 0 (default)
        ADDRESS,    //now have unique device address
        CONFIGURED, //now configured (and not in suspended state)
        SUSPENDED   //no activity for >3ms
    } state_t;

    static state_t state;



    private:

    //registers - all registers use only first 8bits
    enum {
        U1PWRC = 0xBF808480,
        U1IR = 0xBF808600,//no SET, INV
        U1IE = 0xBF808610,
        U1EIR = 0xBF808620,//no SET, INV
        U1EIE = 0xBF808630,
        U1STAT = 0xBF808640,//no SET, INV, CLR
        U1CON = 0xBF808650,
        U1ADDR = 0xBF808660,
        U1BDTP1 = 0xBF808670,
        U1FRML = 0xBF808680, //no SET, INV, CLR
        U1FRMH = 0xBF808690, //no SET, INV, CLR
        U1BDTP2 = 0xBF8086C0,
        U1BDTP3 = 0xBF8086D0,
        U1CNFG1 = 0xBF8086E0,
        U1EP0 = 0xBF808700, U1EP_SPACING = 0x10
     };

};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Usb static functions, vars
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Usb::state_t Usb::state = Usb::DETACHED;
bool Usb::power(POWER e){ return Reg::is_set8(U1PWRC, e); }
void Usb::power(POWER e, bool tf){ Reg::set(U1PWRC, e, tf); }
volatile uint8_t Usb::flags(){ return Reg::val8(U1IR); }
volatile uint8_t Usb::eflags(){ return Reg::val8(U1EIR); }
volatile bool Usb::flag(FLAGS e){ return Reg::is_set8(U1IR, e); }
volatile bool Usb::eflag(EFLAGS e){ return Reg::is_set8(U1EIR, e); }
void Usb::flags_clr(uint8_t v){ Reg::val8(U1IR, v); }
void Usb::eflags_clr(uint8_t v){ Reg::val8(U1EIR, v); }
uint8_t Usb::irqs(){ return Reg::val8(U1IE); }
uint8_t Usb::eirqs(){ return Reg::val8(U1EIE); }
bool Usb::irq(FLAGS e){ return Reg::is_set8(U1IE, e); }
bool Usb::eirq(EFLAGS e){ return Reg::is_set8(U1EIE, e); }
void Usb::irqs(uint8_t v){ Reg::val8(U1IE, v); }
void Usb::eirqs(uint8_t v){ Reg::val8(U1EIE, v); }
void Usb::irq(FLAGS e, bool tf){ Reg::set(U1IE, e); }
void Usb::eirq(EFLAGS e, bool tf){ Reg::set(U1EIE, e); }
volatile uint8_t Usb::stat(){ return Reg::val8(U1STAT); }
volatile bool Usb::control(CONTROL e){ return Reg::is_set8(U1CON, e); }
void Usb::control(CONTROL e, bool tf){ Reg::set(U1CON, e, tf); }
void Usb::control(uint8_t v){ Reg::val8(U1CON, v); }
uint8_t Usb::dev_addr(){ return Reg::val8(U1ADDR) & 127; }
void Usb::dev_addr(uint8_t v){ Reg::clr(U1ADDR, 0x7F); Reg::set(U1ADDR, v); }
uint16_t Usb::frame(){ return (Reg::val8(U1FRMH)<<8) | Reg::val8(U1FRML); }
void Usb::bdt_addr(uint32_t v){
    v = Reg::k2phys(v); //physical address
    Reg::val8(U1BDTP1, v>>8); //512byte aligned (bit0 of this reg unused)
    Reg::val8(U1BDTP2, v>>16);
    Reg::val8(U1BDTP3, v>>24);
}
uint32_t Usb::bdt_addr(){
    return  Reg::p2kseg0( (uint32_t)
    Reg::val8(U1BDTP1)<<8 | Reg::val8(U1BDTP2)<<16 | Reg::val8(U1BDTP3)<<24
    ); //kseg0
}
void Usb::config(CONFIG e, bool tf){ Reg::set(U1CNFG1, e, tf); }
bool Usb::config(CONFIG e){ return Reg::is_set8(U1CNFG1, e); }
void Usb::config(uint8_t v){ Reg::val8(U1CNFG1, v); }
//______________________________________________________________________________


//__inline include______________________________________________________________

#include "UsbEndpt.hpp"

//______________________________________________________________________________
//put here for now
UsbEndpt ep[my_last_endp+1] = {
    {0,UsbEndpt::TRX, 64}, //endpoint 0
    {1,UsbEndpt::TX, 64},
    {2,UsbEndpt::RX, 64},
    {3,UsbEndpt::NONE, 0},
    {4,UsbEndpt::RX, 64}
};
//can create names, like-
// UsbEndpt& ep0 = ep[0];




struct UsbHandlers {
//______________________________________________________________________________
//
// UsbHandlers - handle usb interrupt, call handlers
//______________________________________________________________________________
    static void     UsbISR              (void);
    static void     init                ();
    //state handlers
    static void     detach              (void);
    static void     attach              (void);
};
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// UsbHandlers static functions, vars
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/*..............................................................................
    USB ISR
    declared in user data at top of this file
..............................................................................*/
void USB_ISR(){
    Usb u; Irq ir;
    static Usb::state_t last_state; //keep track of usb state for resumes

    Usb::flags_t flags;
    Usb::eflags_t eflags;
    Usb::stat_t stat;

    flags.all = u.flags();          //get all usb specific irq flags
    eflags.all = u.eflags();        //get all usb specific irq error flags
    stat.all = u.stat();            //get stat reg BEFORE flags cleared
    u.flags_clr(flags.all);         //clear only what we got (1=clear)
    u.eflags_clr(eflags.all);       //clear only what we got (1=clear)
    ir.flag_clr(ir.USB);            //clear usb irq flag

    //ATTACHED->POWERED if vbus_pin high
    if(u.state == u.ATTACHED){
        if(vbus_pin.ison()) u.state = u.POWERED;
        else return; //no power (not sure how we get here)
    }

    //must be >= POWERED

    //if SUSPENDED, check for resume or reset
    if(u.state == u.SUSPENDED){
        if(flags.resume){               //resume
            u.state = last_state;       //back to previous state
            u.irq(u.RESUME, false);     //disable resume irq
            u.irq(u.IDLE, true);        //enable idle (?)
        } else if(! flags.reset){       //if not reset,
            return;                     //still suspended
        }
        //reset or resume, continue below
    }

    //check if need to suspend (idle detected >3ms)
    if (flags.idle){
        last_state = u.state; //save
        u.state = u.SUSPENDED;
        u.irq(u.RESUME, true); //enable resume irq
        u.irq(u.IDLE, false); //disable idle (?)
        //do suspend- whatever is needed
        return;
    }

    //check reset
    //POWERED->DEFAULT, or >=DEFAULT->ATTACHED
    if(flags.reset){ //handle reset condition
        if(u.state == u.POWERED) u.state = u.DEFAULT;
        else {
            UsbHandlers::attach(); //from >=DEFAULT, so attach
            return;
        }
    }

    //in state DEFAULT, ADDRESS, or CONFIGURED
    //(ADDRESS, CONFIGURED set in non-isr code)

    if(flags.error){ //handle errors
    }

    if(flags.stall){ //handle stall
    }

    if(flags.sof){ //handle SOF
    }

    if(flags.token){
        //call only if an endpoint range we are using
        if(stat.endpt <= my_last_endp){
            ep[stat.endpt].token((uint8_t)stat.bdidx);
        }
    }
}

/*..............................................................................
    detach
        power down usb module, usb irq's off
..............................................................................*/
void UsbHandlers::detach(void){
    Usb u; Irq ir;
    u.control(u.USBEN, false);  //disable usb module, detach from bus
    ir.on(ir.USB, false);       //all usb irqs off
    while(u.power(u.BUSY));     //wait for busy bit to clear first
    u.power(u.USBPWR, false);   //usb module off

    u.state = u.DETACHED;
    //wait 100ms+ before attach again (if we are cause of reset/detach and
    //was previously in >= DEFAULT state)
    //if was a usb reset that caused the attach function to run (and this)
    //then the usb host already is in reset and a quick attach will cause no
    //problems (I think that's correct)
}

/*..............................................................................
    attach
        called from init (which first calls detach)
        called from isr (from >=DEFAULT back to ATTACHED)
..............................................................................*/
void UsbHandlers::attach(void){
    Usb u; UsbBuf ubuf; Irq ir;

    detach();                       //all off (regs will reset from off->on)
    u.power(u.USBPWR, true);        //usb on (all regs now reset)

    u.bdt_addr(UsbEndpt::bdt_addr());//set bdt address
    ubuf.reinit();                  //clear all buffers, clear in-use flag

    for(auto& i : ep) i.reinit();   //init (reinit) each UsbEndpt
    ep[0].on(true);                 //and enable endpoint 0

    //enable irqs
    u.irqs(u.STALL|u.IDLE|u.TOKEN|u.SOF|u.ERROR|u.RESET);
    u.eirqs(u.BSTUFF|u.BTMOUT|u.DATSIZ|u.CRC16|u.CRC5|u.PID);
    ir.init(ir.USB, usb_irq_pri, usb_irq_subpri, true);

    u.control(u.USBEN, true);       //enable usb

    //isr takes over
}

/*..............................................................................
    init
        call once to init usb
..............................................................................*/
 void UsbHandlers::init(){
    vbus_pin.digital_in();  //vbus/rb6 to input
    vbus_pin.pulldn(true);  //pulldown when no vbus keeps pin low
    attach();               //init all things
 }




/*
________________________________________________________________________________

random notes
________________________________________________________________________________




to perform a remote-wakeup (if my_remote_wakeup = 1)
    Usb::control(RESUMEN, true);
    DelayCP0 tmr;
    tmr.wait_ms(10);
    Usb::control(RESUMEN, false);

 stall
    setting bstall in descriptor also causes epstall (U1EPn) to set
    stalling the endpoint (both tx/rx)
    to clear the stall, clear bstall, then clear epstall (I think)

 control read transfer
    if data sent in (tx) is less than requested AND the size sent is equal
    to the endpoint's max packet size, a 0byte data packet will also need
    to be sent in the next IN packet to signal no more data
    (a less than max packet signals no more data to send, since the host is
    expecting more data if the last packet was a full packet, it then needs
    to be 'told' there is no more data with a 0byte packet)

 enumeration
 usb bus reset by host controller
 rx setup packet- (DEV)GET_DESCRIPTOR from host
 tx packet- descriptor, number of bytes requested
 8th byte in descriptor contains max packet size for endpoint
 WINDOWS will start status stage once it receives 8 bytes
 (rx packet- 0bytes ? I guess), then resets usb bus

 rx setup packet- (DEV)SET_ADDRESS from host
 tx status packet- 0bytes
 set address (now in ADDRESS state)

 rx setup packet- (DEV)GET_DESCRIPTOR from host
 tx packet- descriptor, number of bytes requested
 rx status packet- 0bytes






*/
