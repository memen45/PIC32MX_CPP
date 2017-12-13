#pragma once

/*=============================================================================
 USB
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Usb {

    public:

    enum FLAGSOTG : uint8_t {
        //U1OTGIR
        ID = 1<<7, MSTIMER = 1<<6 /*no STAT*/, LINE_STABLE = 1<<5,
        ACTIVITY = 1<<4/*no STAT*/, SESSION = 1<<3, BVBUS = 1<<2, AVBUS = 1<<0
    };
    static bool     flag                (FLAGSOTG);
    static void     flag                (FLAGSOTG, bool);
    static void     irq                 (FLAGSOTG, bool);
    static bool     stat                (FLAGSOTG);



    enum OTG : uint8_t {
        DPUP = 1<<7, DMUP = 1<<6, DPDN = 1<<5, DMDN = 1<<4,
        VBUSON = 1<<3, OTGON = 1<<2, VCHARGE = 1<<1, VDISCHARGE = 1<<0
    };
    static void     otg                 (OTG, bool);



    enum FLAGS : uint8_t {
        //U1IR
        STALL = 1<<7, ATTACH = 1<<6, RESUME = 1<<5, IDEL = 1<<4, TOKEN = 1<<3,
        SOF = 1<<2, ERR = 1<<1, RESET = 1<<0, DETACH = 1<<0
    };
    static bool     flag                (FLAGS);
    static void     flag                (FLAGS, bool);
    static void     irq                 (FLAGS, bool);



    enum FLAGSERR : uint8_t {
        //U1IR
        BITSTUFF = 1<<7, BUSMATRIX = 1<<6, DMA = 1<<5, BUSTIMEOUT = 1<<4,
        DATASIZE = 1<<3, CRC16 = 1<<2, CRC5 = 1<<1, EOF = 1<<1, PID = 1<<0
    };
    static bool     flag                (FLAGSERR);
    static void     flag                (FLAGSERR, bool);
    static void     irq                 (FLAGSERR, bool);



    enum POWER : uint8_t {
        PENDING = 1<<7, SLEEPGUARD = 1<<4, BUSY = 1<<3,
        SUSPEND = 1<<1, USBPWR = 1<<0
    };
    static bool     power               (POWER);
    static void     power               (POWER, bool);



    typedef struct __attribute__ ((packed)) {
        uint8_t:2;
        uint8_t ODD:1;
        uint8_t TX:1;
        uint8_t EPNUM:4;
    } stat_t;
    static stat_t   stat                ();



    enum CONTROL : uint8_t {
        JSTATE = 1<<7, SE0 = 1<<6, PKTDIS = 1<<5, TOKBUSY = 1<<5,
        USBRESET = 1<<4, HOSTEN = 1<<3, RESUMEN = 1<<2, PPRESET = 1<<1,
        USBEN = 1<<0, SOFEN = 1<<0
    };
    static bool     control             (CONTROL);
    static void     control             (CONTROL, bool);



    static void     low_speed           (bool);
    static uint8_t  address             ();
    static void     address             (uint8_t);
    static uint16_t frame               ();



    enum TOKPID : uint8_t { SETUP = 13, IN = 9, OUT = 1 };
    static TOKPID   tok_pid             ();
    static uint8_t  tok_ep              ();
    static void     tok_ep              (uint8_t);


    enum SOFVALS : uint8_t {
        SOF64 = 74, SOF32 = 42, SOF16 = 26, SOF8 = 18
    };
    static void     sof_cnt             (SOFVALS);



    static void     bdt_addr            (uint32_t);
    static uint32_t bdt_addr            ();



    enum CONFIG : uint8_t {
        EYETEST = 1<<7, OEMON = 1<<6, SIDLE = 1<<4, LSDEV = 1<<3,
        AUTOSUSP = 1<<0
    };
    static void     config              (CONFIG, bool);
    static bool     config              (CONFIG);


    enum EPN {
        EP0 = 0, EP1, EP2, EP3, EP4, EP5, EP6, EP7, EP8, EP9,
        EP10, EP11, EP12, EP13, EP14, EP15
    };
    enum EP {
        LS = 1<<7, RETRYDIS = 1<<6, /*these 2 HOST EP0 only*/
        CTRLDIS = 1<<4, /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2, ESTALL = 1<<1, HSHAKE = 1<<0
    };
    static void     endp                (EPN, EP, bool);
    static bool     endp                (EPN, EP);

    private:

    //registers - all use only first 8bits
    enum {
        U1OTGIR = 0xBF808440, //no SET, INV - all bits write-1-to-clear
        U1OTGIE = 0xBF808450,
        U1OTGSTAT = 0xBF808460,//no SET, INV, CLR
        U1OTGCON = 0xBF808470,
        U1PWRC = 0xBF808480,
        U1IR = 0xBF808600,//no SET, INV
        U1IE = 0xBF808610,
        U1EIR = 0xBF808620,//no SET, INV
        U1EIE = 0xBF808630,
        U1STAT = 0xBF808640,//no SET, INV, CLR
        U1CON = 0xBF808650,
        U1ADDR = 0xBF808660,
            LSEN = 1<<7,
        U1BDTP1 = 0xBF808670,
        U1FRML = 0xBF808680, //no SET, INV, CLR
        U1FRMH = 0xBF808690, //no SET, INV, CLR
        U1TOK = 0xBF8086A0,
        U1SOF = 0xBF8086B0,
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

bool Usb::flag(FLAGSOTG e){ return Reg::is_set8(U1OTGIR, e); }
void Usb::flag(FLAGSOTG e, bool tf){ Reg::val8(U1OTGIR, 1); } //1 to clear
void Usb::irq(FLAGSOTG e, bool tf){ Reg::set(U1OTGIR, e, tf); }
bool Usb::stat(FLAGSOTG e){ return Reg::is_set8(U1OTGSTAT, e); }
void Usb::otg(OTG e, bool tf){ Reg::set(U1OTGCON, e, tf); }
bool Usb::power(POWER e){ Reg::is_set8(U1PWRC, e); }
void Usb::power(POWER e, bool tf){ Reg::set(U1PWRC, e, tf); }

bool Usb::flag(FLAGS e){ return Reg::is_set8(U1IR, e); }
void Usb::flag(FLAGS e, bool tf){ Reg::val8(U1IR, 1); } //1 to clear
void Usb::irq(FLAGS e, bool tf){ Reg::set(U1IE, e, tf); }

bool Usb::flag(FLAGSERR e){ return Reg::is_set8(U1EIR, e); }
void Usb::flag(FLAGSERR e, bool tf){ Reg::val8(U1EIR, 1); } //1 to clear
void Usb::irq(FLAGSERR e, bool tf){ Reg::set(U1EIE, e, tf); }

Usb::stat_t Usb::stat(){ Reg::val8(U1STAT)>>2; }

bool Usb::control(CONTROL e){ Reg::is_set8(U1CON, e); }
void Usb::control(CONTROL e, bool tf){ Reg::set(U1CON, e, tf); }

void Usb::low_speed(bool tf){ Reg::set(U1ADDR, LSEN, tf); }

uint8_t Usb::address(){ return Reg::val8(U1ADDR) & 127; }
void Usb::address(uint8_t v){ Reg::val8(U1ADDR, v&127); }

uint16_t Usb::frame(){ return (Reg::val8(U1FRMH)<<8) | Reg::val8(U1FRML); }

Usb::TOKPID Usb::tok_pid(){ return (Usb::TOKPID)(Reg::val8(U1TOK)>>4); }
uint8_t Usb::tok_ep(){ return Reg::val8(U1TOK) & 15; }
void Usb::tok_ep(uint8_t v){ Reg::clr(U1TOK, 15); Reg::set(U1TOK, v & 15); }

void Usb::sof_cnt(SOFVALS e){ Reg::val8(U1SOF, e); }

void Usb::bdt_addr(uint32_t v){
    Reg::val8(U1BDTP1, v>>8); //512byte aligned (bit0 of this reg unused)
    Reg::val8(U1BDTP2, v>>16);
    Reg::val8(U1BDTP3, v>>24);
}
uint32_t Usb::bdt_addr(){
    return  Reg::val8(U1BDTP1)<<8 |
            Reg::val8(U1BDTP2)<<16 |
            Reg::val8(U1BDTP3)<<24;
}

void Usb::config(CONFIG e, bool tf){ Reg::set(U1CNFG1, e, tf); }
bool Usb::config(CONFIG e){ return Reg::is_set8(U1CNFG1, e); }

void Usb::endp(EPN n, EP e, bool tf){ Reg::set(U1EP0+n*U1EP_SPACING, e, tf); }
bool Usb::endp(EPN n, EP e){ return Reg::is_set8(U1EP0+n*U1EP_SPACING, e); }