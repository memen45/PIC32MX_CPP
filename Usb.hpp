#pragma once

/*=============================================================================
 USB peripheral - pic32mmxxxxGPMxxx
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"


class Usb {

    public:

    /*=========================================================================
     flags, interrupts
        FLAGS enum contains all flag names, with error names shifted left 8bits
            this enaum also doubles for irq names

        flags();                -return val of all flags (uin16_t)
        flag(FLAGS);            -get specified flag
        flags_clr();            -clear all flags
        flags_clr(uint16_t);    -clear flag(s) as bitmask (1=clear)
        flag_clr(FLAGS);        -clear one flag

        irqs();                 -return val of all irqs (uin16_t)
        irq(FLAGS);             -get specified irq
        irqs(uint16_t);         -set value (all)
        irq(FLAGS, bool);       -one irq on/off (true=on)
    =========================================================================*/

    enum FLAGS : uint16_t { //unite all flags, irq's - | err<<8 | reg |
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

        //U1EIR, U1EIE (+32bytes from U1IR, U1IE)
        //these bits shifted up by 8
        BITSTUFF = 1<<15,
        BUSMATRIX = 1<<14,
        DMA = 1<<13,
        BUSTIMEOUT = 1<<12,
        DATASIZE = 1<<11,
        CRC16 = 1<<10,
        CRC5 = 1<<9,
        EOF = 1<<9,
        PID = 1<<8
    };

    static uint16_t flags               ();             //get all
    static bool     flag                (FLAGS);        //get one
    static void     flags_clr           ();             //clear all
    static void     flags_clr           (uint16_t);     //clear one or more
    static void     flag_clr            (FLAGS);        //clear one

    static uint16_t irqs                ();             //get all
    static bool     irq                 (FLAGS);        //get one
    static void     irqs                (uint16_t);     //set value (all)
    static void     irq                 (FLAGS, bool);  //irq on/off


/////////////////////////////////////////////////////////////
static const uint16_t irq_list = (
    STALL|IDLE|TOKEN|SOF|ERR|RESET |            //normal
    BITSTUFF|BUSTIMEOUT|DATASIZE|CRC16|CRC5|PID //error
);
/////////////////////////////////////////////////////////////

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



    static uint8_t  stat                (); //only valid when flag(TOKEN)=1



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


    enum BD {
         UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5, NINC = 1<<4,
         DTS = 1<<3, BSTALL = 1<<2
     };

    //address of (up to)512byte BDT buffer (512byte aligned)
    //(16bdt's * 2(in/out) * 2(odd/even) * 8bytes = 512bytes max)
    static void     bdt_addr            (uint32_t);
    static uint32_t bdt_addr            ();
    static void     bdt_clr             (); //clear all table entries

    static void     bd_control          (uint8_t, BD, bool);
    static bool     bd_control          (uint8_t, BD);
    static uint8_t  bd_pid              (uint8_t);
    static void     bd_addr             (uint8_t, uint32_t);
    static uint32_t bd_addr             (uint8_t);
    static uint16_t bd_count            (uint8_t);
    static void     bd_count            (uint8_t, uint16_t);


/////////////////////////////////////////////////////////////
static const uint8_t maxn_endp = 3; //0-3
typedef union __attribute__((packed)) {
    struct { unsigned :2; unsigned pid:4; unsigned :2; };
    struct { unsigned :16; uint16_t count; };
    struct { uint32_t dat; uint32_t addr; };
    uint64_t all;
} Bdt_entry_t;

static volatile Bdt_entry_t bdt[(maxn_endp+1)*4]
    __attribute__ ((aligned (512)));


/////////////////////////////////////////////////////////////



    enum CONFIG : uint8_t {
        EYETEST = 1<<7,
        OEMON = 1<<6,
        SIDLE = 1<<4,
        LSDEV = 1<<3,
        AUTOSUSP = 1<<0
    };

    static void     config              (CONFIG, bool);
    static bool     config              (CONFIG);
    static void     config_clr          ();



    enum EPN {
        EP0 = 0, EP1, EP2, EP3, EP4, EP5, EP6, EP7, EP8, EP9,
        EP10, EP11, EP12, EP13, EP14, EP15
    };
    enum EP {
        LS = 1<<7,
        RETRYDIS = 1<<6, /*these 2 HOST EP0 only*/
        CTRLDIS = 1<<4, /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3,
        TXEN = 1<<2,
        ESTALL = 1<<1,
        HSHAKE = 1<<0
    };

    static void     endp                (EPN, EP, bool);
    static bool     endp                (EPN, EP);
    static void     endp_clr            (EPN);
    static void     endps_clr           ();






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
        U1CNFG1 = 0xBF8086E0,
        U1EP0 = 0xBF808700, U1EP_SPACING = 0x10
        //U1EP1 0xBF808710 - U1EP15 0xBF8087F0
     };

};


/*=============================================================================
 all functions inline
=============================================================================*/

bool Usb::power(POWER e){ Reg::is_set8(U1PWRC, e); }
void Usb::power(POWER e, bool tf){ Reg::set(U1PWRC, e, tf); }


//get all
uint16_t Usb::flags(){ return Reg::val8(U1EIR)<<8 | Reg::val8(U1IR); }
//get one
bool Usb::flag(FLAGS e){
    return e < 256 ? Reg::is_set8(U1IR, e) : Reg::is_set8(U1EIR, e>>8);
}
//clear all
void Usb::flags_clr(){ Reg::val8(U1IR, 255); Reg::val8(U1EIR, 255); }
//clear one or more
void Usb::flags_clr(uint16_t v){ Reg::val8(U1EIR, v>>8); Reg::val8(U1IR, v); }
//clear one
void Usb::flag_clr(FLAGS e){
    if(e < 256) Reg::val8(U1IR, e);
    else Reg::val8(U1EIR, e>>8);
}

//get all
uint16_t Usb::irqs(){ return Reg::val8(U1EIE)<<8 | Reg::val8(U1IE); }
//get one
bool Usb::irq(FLAGS e){
    return e < 256 ? Reg::is_set8(U1IE, e) : Reg::is_set8(U1EIE, e>>8);
}
//set value
void Usb::irqs(uint16_t v){ Reg::val8(U1IE, v); Reg::val8(U1EIE, v>>8); }
//one on/off
void Usb::irq(FLAGS e, bool tf){
    if(e < 256) Reg::set(U1IE, e); else Reg::set(U1EIE, e>>8);
}


uint8_t Usb::stat(){ Reg::val8(U1STAT); }

bool Usb::control(CONTROL e){ Reg::is_set8(U1CON, e); }
void Usb::control(CONTROL e, bool tf){ Reg::set(U1CON, e, tf); }

uint8_t Usb::address(){ return Reg::val8(U1ADDR) & 127; }
void Usb::address(uint8_t v){ Reg::val8(U1ADDR, v&127); }

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
void Usb::bdt_clr(){ for(auto& b : bdt ) b.all = 0; }


//for bd_x functions
//caller needs to use correct n offset -> n+(TX/RX<<3)+(EVEN/ODD<<2)
void Usb::bd_control(uint8_t n, BD e, bool tf){
    if( tf ) bdt[n].dat |= e; else bdt[n].dat &= ~e;
}
bool Usb::bd_control(uint8_t n, BD e){ return bdt[n].dat & e; }
uint8_t Usb::bd_pid(uint8_t n){ return bdt[n].pid; }
void Usb::bd_addr(uint8_t n, uint32_t v){ bdt[n].addr = Reg::k2phys(v); }
uint32_t Usb::bd_addr(uint8_t n){ return Reg::p2kseg0(bdt[n].addr); }
uint16_t Usb::bd_count(uint8_t n){ return bdt[n].count; }
void Usb::bd_count(uint8_t n, uint16_t v){ bdt[n].count = v; }


void Usb::config(CONFIG e, bool tf){ Reg::set(U1CNFG1, e, tf); }
bool Usb::config(CONFIG e){ return Reg::is_set8(U1CNFG1, e); }
void Usb::config_clr(){ Reg::set(U1CNFG1, 0); }

void Usb::endp(EPN n, EP e, bool tf){ Reg::set(U1EP0+n*U1EP_SPACING, e, tf); }
bool Usb::endp(EPN n, EP e){ return Reg::is_set8(U1EP0+n*U1EP_SPACING, e); }
void Usb::endp_clr(EPN n){ Reg::val8(U1EP0+n*U1EP_SPACING, 0); }
void Usb::endps_clr(){
    for( uint8_t e = EP0; e <= EP15; e++ ){ endp_clr((EPN)e); }
}







/*
 void Usb::init(){

    Irq::on(Irq::USB, false);   //usb irq off

    power(USBPWR, false);       //usb off
    while(power(BUSY));         //wait for busy bit to clear

    flags_clear();              //clear all flags
    endps_clr();                //clear all endpoints
    config_clr();               //(FS)
    power(USBPWR, true);        //usb on
    bdt_addr(bdt_table);        //set bdt table address
    bdt_clr();                  //clear all bdt entries
    control(PPRESET, true);     //reset ping pong pointers
    address(0);                 //set adddress to 0
    control(PKTDIS, false);     //enable pkt processing
    control(PPRESET, false);    //stop reset ping pong pointers

    irqs(irq_list);             //set irq_list to list of irqs to enable

    control(USBEN, true);       //usb enable
 }
 */

