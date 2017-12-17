#pragma once

/*=============================================================================
 USB peripheral - pic32mmxxxxGPMxxx
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

// each entry in the BDT table has a class instance of Bdt, created in Usb class
// (array of maxn_endp class instances)
// each class instance handles its own entry (in bdt table), buffer (64 bytes),
// and endpoint register control
// for each endpoint there are 4 Bdt instances (rxeven, rxodd, txeven, txodd)
// access to these instances is through Usb:: via bdt_handler[n]
// where n = endpoint number<<2|trx>>1|evodd
// for Usb::stat(), use Usb::bdt_handler[Usb::stat()>>2]
class UsbBdt {

    public:

    enum BD {
         UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
         NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<2
     };

    enum EP {
        LS = 1<<7, /*HOST mode and U1EP0 only*/
        RETRYDIS = 1<<6, /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4, /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2, ESTALL = 1<<1, HSHAKE = 1<<0
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

    void        init        (volatile bdt_t* entry_ptr, uint8_t n);
    void        control     (uint8_t v, bool tf);
    bool        control     (uint8_t v);
    uint8_t     pid         ();
    uint16_t    count       ();
    void        count       (uint16_t v);
    void        endp        (EP e, bool tf);
    bool        endp        (EP e);
    void        endp        (uint8_t);


    private:

    enum { U1EP0 = 0xBF808700, U1EP_SPACING = 0x10 };
    //U1EP1 0xBF808710 - U1EP15 0xBF8087F0

    uint8_t m_endp_num;
    uint8_t m_entry_num;
    volatile uint8_t m_buffer[64]{0};
    volatile bdt_t* m_entry_ptr;
};

//called by Usb to init this UsbBdt instance
//get entry address, endpoint number
//clear entry (4*16bits= 8bytes = 2x32bits)
//clear buffer
void UsbBdt::init(volatile bdt_t* entry_ptr, uint8_t n){
    m_entry_ptr = &entry_ptr[n];
    m_entry_num = n;
    m_endp_num = n/4; //4 entries per endpoint n
    m_entry_ptr->all = 0;
    for(auto& b : m_buffer) b = 0;
    m_entry_ptr->addr = Reg::k2phys(m_buffer);
}

void UsbBdt::control(uint8_t v, bool tf){
    if( tf ) m_entry_ptr->control |= v;
    else m_entry_ptr->control &= ~v;
}
bool UsbBdt::control(uint8_t v){ return m_entry_ptr->control & v; }

uint8_t UsbBdt::pid(){ return m_entry_ptr->pid; }

uint16_t UsbBdt::count(){ return m_entry_ptr->count; }
void UsbBdt::count(uint16_t v){ m_entry_ptr->count = v; }

void UsbBdt::endp(EP e, bool tf){ Reg::set(U1EP0+m_endp_num*U1EP_SPACING, e, tf); }
bool UsbBdt::endp(EP e){ return Reg::is_set8(U1EP0+m_endp_num*U1EP_SPACING, e); }
void UsbBdt::endp(uint8_t v){ Reg::val8(U1EP0+m_endp_num*U1EP_SPACING, v); }









class Usb {

    public:

    /*=========================================================================
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
    =========================================================================*/

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


    /*=========================================================================
     power
        POWER enum contains all power names

        power(POWER);           -return POWER bit is set
        power(POWER, bool);     -set/clr specified POWER bit (true=on)
    =========================================================================*/

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



    static uint8_t  stat                (); //only valid when TOKEN flag set



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



    static uint8_t  address             ();
    static void     address             (uint8_t);
    static uint16_t frame               ();




    //address of (up to)512byte BDT buffer (512byte aligned)
    //(16bdt's * 2(in/out) * 2(odd/even) * 8bytes = 512bytes max)
    //table
    static void     bdt_addr            (uint32_t);
    static uint32_t bdt_addr            ();




/////////////////////////////////////////////////////////////
static const uint8_t maxn_endp = 1; //0,1
/////////////////////////////////////////////////////////////

static volatile UsbBdt::bdt_t bdt[(maxn_endp+1)]
    __attribute__ ((aligned (512)));

 static UsbBdt bdt_handler[(maxn_endp+1)*4]; //4 per endpoint

 static void bdt_handler_init();




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



    private:

    //registers - all use only first 8bits
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
        U1CNFG1 = 0xBF8086E0
     };

};


/*=============================================================================
 all functions inline
=============================================================================*/

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

//init UsbBdt bdt_handler[n]
void Usb::bdt_handler_init(){
    uint8_t n = 0;
    for(auto& h : bdt_handler) h.init( bdt, n++ );
}




/*
 void Usb::init(){

    Irq::on(Irq::USB, false);   //usb irq off

    power(USBPWR, false);       //usb off
    while(power(BUSY));         //wait for busy bit to clear

    flags_clear();              //clear all flags
    endps_clr();                //clear all endpoints
    config(0);                  //(FS)
    power(USBPWR, true);        //usb on
    bdt_class_init();           //clear all bdt entries, init class (each entry)
    bdt_addr(bdt_table);        //set bdt table address

    control(0);
    control(PPRESET, true);     //reset ping pong pointers
    address(0);                 //set address to 0
    control(0);                 //stop reset ping pong pointers

    irqs(STALL|IDLE|TOKEN|SOF|ERR|RESET);
    eirqs(BITSTUFF|BUSTIMEOUT|DATASIZE|CRC16|CRC5|PID);

    control(USBEN, true);       //usb enable

    Irq::init(Irq::Usb, 5, 0, true);
 }
 */

