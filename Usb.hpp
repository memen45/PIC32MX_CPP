#pragma once

/*=============================================================================
 USB peripheral - pic32mmxxxxGPMxxx
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

///////////////////////////////////////////////////////////////////////////////
/////// user provided data ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const uint8_t my_last_endp = 1;     //last endpoint number used
static const uint8_t my_n_endp = 2;        //number of endpoints used
static const uint8_t my_buffer_size = 64;  //size of buffers (all same)
static const uint8_t my_buffer_count = 8;  //number of buffers in buffer pool
///////////////////////////////////////////////////////////////////////////////



struct UsbBuf {
//==============================================================================
// all usb buffers come from here
// 8 * 64byte buffers - ALL are 64bytes in length
// caller requests buffer via -
//   volatile uint8_t* mybuf = UsbBuf::giveme();
//   if(!mybuf) no_free_buffers- try again
// caller returns buffer when done
//   UsbBuf::giveup(mybuf);
// to reinit buffers (release all, and clear)-
//   UsbBuf::reinit();
//==============================================================================
    static void                 reinit();
    static volatile uint8_t*    get();
    static void                 release(volatile uint8_t*);
    static uint8_t              buf_len();

    typedef struct {
        bool inuse;
        uint8_t buf[my_buffer_size];
    } buf_t;
};
//==============================================================================
// UsbBuf static functions, vars
//==============================================================================
static volatile UsbBuf::buf_t m_buffers[my_buffer_count] = {0};

//clear buffers, clear inuse flag
void UsbBuf::reinit(){
    for(auto& i : m_buffers){
        i.inuse = false;
        for(auto& j : i.buf) j = 0;
    }
}
//get an unused buffer (address), return 0 if none available (caller checks 0)
volatile uint8_t* UsbBuf::get(){
    for(auto& i : m_buffers){
        if(i.inuse) continue;
        i.inuse = true;
        return i.buf;
    }
    return 0;
}
//return a buffer for use
void UsbBuf::release(volatile uint8_t* p){
    for(auto& i : m_buffers){
        if(p!=i.buf) continue;
        i.inuse = false;
        return;
    }
}
//size of buffer
uint8_t UsbBuf::buf_len() { return my_buffer_size; }
//==============================================================================







struct UsbBdt {
//==============================================================================
// all BDT table functions
// bdt table allocated here, Usb:: functions set address of bdt table
// init functions also return address of table (init_all) or table entry (init)
// something like-
//  uint32_t* ba = UsbBdt::init_all();
//  Usb::bdt_addr(ba);
//==============================================================================
    enum CTRL {
        UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
        NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<1
    };

    typedef union {
        struct { uint32_t dat; uint32_t addr; };
        struct { unsigned :16; uint16_t count; };
        struct {
            unsigned :2;
            unsigned bstall:1;
            unsigned dts:1;
            unsigned ninc:1;
            unsigned keep:1;
            unsigned data01:1;
            unsigned uown:1;
        };
        struct { unsigned :2; unsigned pid:4; };
        uint8_t control;
        uint64_t all;
    } bdt_t;

    void*       init        (uint8_t);
    void*       init_all    ();
    void        addr        (uint8_t, uint8_t*);
    uint8_t*    addr        (uint8_t);
    void        control     (uint8_t, CTRL, bool);
    void        control     (uint8_t, uint8_t);
    bool        control     (uint8_t, CTRL);
    //control - specific functions
    bool        uown        (uint8_t);
    bool        data01      (uint8_t);
    bool        uown        (uint8_t, bool);
    bool        data01      (uint8_t, bool);

    uint8_t     pid         (uint8_t);
    uint16_t    count       (uint8_t);
    void        count       (uint8_t, uint16_t);
};
//==============================================================================
// UsbBdt static functions, vars
//==============================================================================
static volatile UsbBdt::bdt_t bdt[(my_last_endp+1)*4]
    __attribute__ ((aligned (512)));

//init single bdt table entry (epn<<3|dir<<1|pp)
void* UsbBdt::init(uint8_t n){
    if(n > (my_last_endp*4)) return 0; //n too high, do nothing
    bdt[n].all = 0;
    return (void*)&bdt[n];
}
//init all bdt table entries (4 per endpoint)
void* UsbBdt::init_all(){
    for(auto& i : bdt) i.all = 0;
    return (void*)bdt;
}

void UsbBdt::addr(uint8_t n, uint8_t* v){ bdt[n].addr = Reg::k2phys(v); }
uint8_t* UsbBdt::addr(uint8_t n){ return (uint8_t*)Reg::p2kseg0(bdt[n].addr); }

void UsbBdt::control(uint8_t n, CTRL e, bool tf){
    if( tf ) bdt[n].control |= e;
    else bdt[n].control &= ~e;
}
void UsbBdt::control(uint8_t n, uint8_t v){ bdt[n].control = v; }
bool UsbBdt::control(uint8_t n, CTRL e){ return bdt[n].control & e; }
bool UsbBdt::uown(uint8_t n){ return bdt[n].uown; }
bool UsbBdt::data01(uint8_t n){ return bdt[n].data01; }
bool UsbBdt::uown(uint8_t n, bool tf){ bdt[n].uown = tf; }
bool UsbBdt::data01(uint8_t n, bool tf){ bdt[n].data01 = tf; }


uint8_t UsbBdt::pid(uint8_t n){ return bdt[n].pid; }

uint16_t UsbBdt::count(uint8_t n){ return bdt[n].count; }
void UsbBdt::count(uint8_t n, uint16_t v){ bdt[n].count = v; }










class Usb {
//==============================================================================

//==============================================================================
    public:

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

    enum FLAGS : uint16_t {
        //U1IR, U1IE
        STALL = 1<<7,
        ATTACH = 1<<6,
        RESUME = 1<<5,
        IDLE = 1<<4,
        TOKEN = 1<<3,
        SOF = 1<<2,
        ERR = 1<<1,
        RESET = 1<<0,
        DETACH = 1<<0,
        ALLFLAGS = 255
    };

    enum EFLAGS : uint8_t {
        //U1EIR, U1EIE
        BITSTUFF = 1<<7,
        BUSMATRIX = 1<<6,
        DMA = 1<<5,
        BUSTIMEOUT = 1<<4,
        DATASIZE = 1<<3,
        CRC16 = 1<<2,
        CRC5 = 1<<1,
        EOF = 1<<1,
        PID = 1<<0,
        ALLEFLAGS = 255
    };

    static uint8_t  flags               ();             //get all
    static bool     flag                (FLAGS);        //get one
    static void     flags_clr           (uint8_t);      //clear one or more

    static uint8_t  irqs                ();             //get all
    static bool     irq                 (FLAGS);        //get one
    static void     irqs                (uint8_t);      //set value (all)
    static void     irq                 (FLAGS, bool);  //irq on/off

    static uint8_t  eflags              ();             //get all
    static bool     eflag               (EFLAGS);       //get one
    static void     eflags_clr          (uint8_t);      //clear one or more

    static uint8_t  eirqs               ();             //get all
    static bool     eirq                (EFLAGS);       //get one
    static void     eirqs               (uint8_t);      //set value (all)
    static void     eirq                (EFLAGS, bool); //eirq on/off


    /*..........................................................................
     power
        POWER enum contains all power names

        power(POWER);           -return POWER bit is set
        power(POWER, bool);     -set/clr specified POWER bit (true=on)
    ..........................................................................*/

    enum POWER : uint8_t {
        //U1PWRC
        PENDING = 1<<7,
        SLEEPGUARD = 1<<4,
        BUSY = 1<<3,
        SUSPEND = 1<<1,
        USBPWR = 1<<0
    };

    static bool     power               (POWER);
    static void     power               (POWER, bool);

    /*..........................................................................
     stat
        only valid when TOKEN flag set
    ..........................................................................*/

    static uint8_t  stat                ();


    /*..........................................................................
     control

    ..........................................................................*/
    enum CONTROL : uint8_t {
        JSTATE = 1<<7,
        SE0 = 1<<6,
        PKTDIS = 1<<5,
        TOKBUSY = 1<<5,
        USBRESET = 1<<4,
        HOSTEN = 1<<3,
        RESUMEN = 1<<2,
        PPRESET = 1<<1,
        USBEN = 1<<0,
        SOFEN = 1<<0
    };

    static bool     control             (CONTROL);
    static void     control             (CONTROL, bool);


    /*..........................................................................
     address
        usb device address get/set
    ..........................................................................*/
    static uint8_t  address             ();
    static void     address             (uint8_t);


    /*..........................................................................
     frame
        get frame number
    ..........................................................................*/
    static uint16_t frame               ();


    /*..........................................................................
     bdt_addr
        get/set bdt table address
    ..........................................................................*/
    static void     bdt_addr            (uint32_t);
    static uint32_t bdt_addr            ();


    /*..........................................................................
     config
        get/set config register
    ..........................................................................*/
    enum CONFIG : uint8_t {
        EYETEST = 1<<7,
        OEMON = 1<<6,
        SIDLE = 1<<4,
        LSDEV = 1<<3,
        AUTOSUSP = 1<<0
    };

    static void     config              (CONFIG, bool); //set 1bit
    static bool     config              (CONFIG);       //get 1bit
    static void     config              (uint8_t);      //set reg val


    /*..........................................................................
     endp
        get/set endpoint n
    ..........................................................................*/
    enum EPV {
        LS = 1<<7,          /*HOST mode and U1EP0 only*/
        RETRYDIS = 1<<6,    /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4,     /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2,
        ESTALL = 1<<1, HSHAKE = 1<<0
    };

    static void     endp                (uint8_t, EPV, bool);
    static bool     endp                (uint8_t, EPV);
    static void     endp                (uint8_t, uint8_t);




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
        U1BDTP2 = 0xBF806C0,
        U1BDTP3 = 0xBF806D0,
        U1CNFG1 = 0xBF8086E0,
        U1EP0 = 0xBF808700, U1EP_SPACING = 0x10
     };

};
//==============================================================================
// Usb static functions, vars
//==============================================================================
bool Usb::power(POWER e){ Reg::is_set8(U1PWRC, e); }
void Usb::power(POWER e, bool tf){ Reg::set(U1PWRC, e, tf); }
//get all
uint8_t Usb::flags(){ return Reg::val8(U1IR); }
uint8_t Usb::eflags(){ return Reg::val8(U1EIR); }
//get one
bool Usb::flag(FLAGS e){ Reg::is_set8(U1IR, e); }
bool Usb::eflag(EFLAGS e){ Reg::is_set8(U1EIR, e); }
//clear one or more
void Usb::flags_clr(uint8_t v){ Reg::val8(U1IR, v); }
void Usb::eflags_clr(uint8_t v){ Reg::val8(U1EIR, v); }
//get all
uint8_t Usb::irqs(){ return Reg::val8(U1IE); }
uint8_t Usb::eirqs(){ return Reg::val8(U1EIE); }
//get one
bool Usb::irq(FLAGS e){ Reg::is_set8(U1IE, e); }
bool Usb::eirq(EFLAGS e){ Reg::is_set8(U1EIE, e); }
//set value
void Usb::irqs(uint8_t v){ Reg::val8(U1IE, v); }
void Usb::eirqs(uint8_t v){ Reg::val8(U1EIE, v); }
//one on/off
void Usb::irq(FLAGS e, bool tf){ Reg::set(U1IE, e); }
void Usb::eirq(EFLAGS e, bool tf){ Reg::set(U1EIE, e); }

uint8_t Usb::stat(){ Reg::val8(U1STAT); }

bool Usb::control(CONTROL e){ Reg::is_set8(U1CON, e); }
void Usb::control(CONTROL e, bool tf){ Reg::set(U1CON, e, tf); }

uint8_t Usb::address(){ return Reg::val8(U1ADDR) & 127; }
void Usb::address(uint8_t v){ Reg::val8(U1ADDR, v & 127); }

uint16_t Usb::frame(){ return (Reg::val8(U1FRMH)<<8) | Reg::val8(U1FRML); }

void Usb::bdt_addr(uint32_t v){
    v = Reg::k2phys(v); //physical address
    Reg::val8(U1BDTP1, v>>8); //512byte aligned (bit0 of this reg unused)
    Reg::val8(U1BDTP2, v>>16);
    Reg::val8(U1BDTP3, v>>24);
}
uint32_t Usb::bdt_addr(){
    return  Reg::p2kseg0(
                Reg::val8(U1BDTP1)<<8 |
                Reg::val8(U1BDTP2)<<16 |
                Reg::val8(U1BDTP3)<<24
            ); //kseg0
}

void Usb::config(CONFIG e, bool tf){ Reg::set(U1CNFG1, e, tf); }
bool Usb::config(CONFIG e){ return Reg::is_set8(U1CNFG1, e); }
void Usb::config(uint8_t v){ Reg::val8(U1CNFG1, v); }

void Usb::endp(uint8_t n, EPV e, bool tf){
    Reg::set(U1EP0+(n&15)*U1EP_SPACING, e, tf);
}
bool Usb::endp(uint8_t n, EPV e){
    return Reg::is_set8(U1EP0+(n&15)*U1EP_SPACING, e);
}
void Usb::endp(uint8_t n, uint8_t v){
    Reg::val8(U1EP0+(n&15)*U1EP_SPACING, v);
}
//==============================================================================



/*
 void Usb::init(){

    UsbBdt ubdt;

    Irq::on(Irq::USB, false);   //usb irq off

    power(USBPWR, false);       //usb off
    while(power(BUSY));         //wait for busy bit to clear

    flags_clear();              //clear all flags
    endps_clr();                //clear all endpoints
    config(0);                  //(FS)
    power(USBPWR, true);        //usb on
                                //clear all bdt entries
    bdt_addr(ubdt.init_all());  //and set bdt table address

    control(0);
    control(PPRESET, true);     //reset ping pong pointers
    address(0);                 //set address to 0
    control(0);                 //stop reset ping pong pointers

    endp(0, RXEN|TXEN|HSHK);
    ubdt.control(0|0|0, ubdt.UOWN|ubdt.DATA01|ubdt.BSTALL);

    irqs(STALL|IDLE|TOKEN|SOF|ERR|RESET);
    eirqs(BITSTUFF|BUSTIMEOUT|DATASIZE|CRC16|CRC5|PID);

    control(USBEN, true);       //usb enable

    Irq::init(Irq::Usb, 5, 0, true);
 }
 */

