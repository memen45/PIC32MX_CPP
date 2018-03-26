//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "Usb.hpp"
#include "Reg.hpp"
#include "UsbConfig.hpp"
#include "Pins.hpp"

//registers - all registers use only first 8bits
enum {
    U1OTGIR = 0xBF808440,   //no SET, INV - all bits write-1-to-clear
    U1OTGIE = 0xBF808450,
    U1OTGSTAT = 0xBF808460, //no SET, INV, CLR
    U1OTGCON = 0xBF808470,
    U1PWRC = 0xBF808480,
    U1IR = 0xBF808600,      //no SET, INV
    U1IE = 0xBF808610,
    U1EIR = 0xBF808620,     //no SET, INV
    U1EIE = 0xBF808630,
    U1STAT = 0xBF808640,    //no SET, INV, CLR
        ENDPT_SHIFT = 4, ENDPT_MASK = 15,
        DIR = 3,
        PPBI = 2,
    U1CON = 0xBF808650,
    U1ADDR = 0xBF808660,
        LSEN = 7,
    U1BDTP1 = 0xBF808670,
    U1FRML = 0xBF808680,    //no SET, INV, CLR
    U1FRMH = 0xBF808690,    //no SET, INV, CLR
    U1TOK = 0xBF8086A0,
    U1SOF = 0xBF8086B0,
    U1BDTP2 = 0xBF8086C0,
    U1BDTP3 = 0xBF8086D0,
    U1CNFG1 = 0xBF8086E0,
    U1EP0 = 0xBF808700, U1EP_SPACING = 0x10
};


Pins vbus_pin(UsbConfig::vbus_pin_n, Pins::INPD);

//U1OTGIR/IE, U1IR/IE, U1EIR/IE
//=============================================================================
    uint32_t    Usb::flags              ()
//=============================================================================
{
    return Reg::val8(U1OTGIR)<<16 bitor
           Reg::val8(U1EIR)<<8 bitor
           Reg::val8(U1IR);
}

//=============================================================================
    bool        Usb::flag               (FLAGS e)
//=============================================================================
{
    return flags() bitand e;
}

//=============================================================================
    void        Usb::flags_clr          (uint32_t v)
//=============================================================================
{
    Reg::val(U1OTGIR, (uint8_t)(v>>16));
    Reg::val(U1EIR, (uint8_t)(v>>8));
    Reg::val(U1IR, (uint8_t)v);
}

//=============================================================================
    uint32_t    Usb::irqs               ()
//=============================================================================
{
    return Reg::val8(U1OTGIE)<<16 bitor
           Reg::val8(U1EIE)<<8 bitor
           Reg::val8(U1IE);
}

//=============================================================================
    bool        Usb::irq                (FLAGS e)
//=============================================================================
{
    return irqs() bitand e;
}

//=============================================================================
    void        Usb::irqs               (uint32_t v)
//=============================================================================
{
    Reg::val(U1OTGIE, (uint8_t)(v>>16));
    Reg::val(U1EIE, (uint8_t)(v>>8));
    Reg::val(U1IE, (uint8_t)v);
}

//=============================================================================
    void        Usb::irq                (FLAGS e, bool tf)
//=============================================================================
{
    if(e > (1<<15)) Reg::setbit(U1OTGIE, e>>16, tf);
    else if(e > (1<<7))Reg::setbit(U1EIE, e>>8, tf);
    else Reg::setbit(U1IE, e, tf);
}


//U1OTGSTAT
//=============================================================================
    uint32_t    Usb::otg_stat           ()
//=============================================================================
{
    return Reg::val8(U1OTGSTAT)<<16;
}

//=============================================================================
    bool        Usb::otg_stat           (FLAGS e)
//=============================================================================
{
    return Reg::anybit(U1OTGSTAT, e>>16);
}

//U1OTGCON
//=============================================================================
    void        Usb::otg                (OTG e, bool tf)
//=============================================================================
{
    Reg::setbit(U1OTGCON, e, tf);
}

//=============================================================================
    void        Usb::otg                (uint8_t v)
//=============================================================================
{
    Reg::val(U1OTGCON, v);
}

//U1PWRC
//=============================================================================
    bool        Usb::power              (POWER e)
//=============================================================================
{
    return Reg::anybit(U1PWRC, e);
}

//=============================================================================
    void        Usb::power              (POWER e, bool tf)
//=============================================================================
{
    Reg::setbit(U1PWRC, e, tf);
}



//U1STAT
//=============================================================================
    uint8_t     Usb::stat               ()
//=============================================================================
{
    return Reg::val8(U1STAT)>>2;
}

//U1CON
//=============================================================================
    bool        Usb::control            (CONTROL e)
//=============================================================================
{
    return Reg::anybit(U1CON, e);
}

//=============================================================================
    void        Usb::control            (CONTROL e, bool tf)
//=============================================================================
{
    Reg::setbit(U1CON, e, tf);
}

//=============================================================================
    void        Usb::control            (uint8_t v)
//=============================================================================
{
    Reg::val(U1CON, v);
}

//U1ADDR
//=============================================================================
    void        Usb::low_speed          (bool tf)
//=============================================================================
{
    Reg::setbit(U1ADDR, 1<<LSEN, tf);
}

//=============================================================================
    uint8_t     Usb::dev_addr           ()
//=============================================================================
{
    return Reg::val8(U1ADDR) bitand 127;
}

//=============================================================================
    void        Usb::dev_addr           (uint8_t v)
//=============================================================================
{
    Reg::clrbit(U1ADDR, 127);
    Reg::setbit(U1ADDR, v bitand 127);
}

//U1FRML,H
//=============================================================================
    uint16_t    Usb::frame              ()
//=============================================================================
{
    return (Reg::val8(U1FRMH)<<8) bitor Reg::val8(U1FRML);
}

//U1TOK
//=============================================================================
    void        Usb::tok_pid            (TOKPID e)
//=============================================================================
{
    Reg::clrbit(U1TOK, 15<<4);
    Reg::setbit(U1TOK, e);
}

//=============================================================================
    void        Usb::tok_ep             (uint8_t v)
//=============================================================================
{
    Reg::clrbit(U1TOK, 15);
    Reg::setbit(U1TOK, v bitand 15);
}

//U1SOF
//=============================================================================
    void        Usb::sof_cnt            (SOFVALS e)
//=============================================================================
{
    Reg::val(U1SOF, e);
}

//U1BDTP1,2,3
//=============================================================================
    void        Usb::bdt_addr           (uint32_t v)
//=============================================================================
{
    v = Reg::k2phys(v); //physical address
    Reg::val(U1BDTP1, (uint8_t)(v>>8)); //512byte aligned (bit0 unused)
    Reg::val(U1BDTP2, (uint8_t)(v>>16));
    Reg::val(U1BDTP3, (uint8_t)(v>>24));
}

//=============================================================================
    uint32_t    Usb::bdt_addr           ()
//=============================================================================
{
    uint32_t ret =
        Reg::val8(U1BDTP3)<<24 bitor
        Reg::val8(U1BDTP2)<<16 bitor
        Reg::val8(U1BDTP1)<<8;
    return  Reg::p2kseg0(ret); //virtual address i kseg0
}

//U1CNFG1
//=============================================================================
    void        Usb::config             (CONFIG e, bool tf)
//=============================================================================
{
    Reg::setbit(U1CNFG1, e, tf);
}

//=============================================================================
    bool        Usb::config             (CONFIG e)
//=============================================================================
{
    return Reg::anybit(U1CNFG1, e);
}

//=============================================================================
    void        Usb::config             (uint8_t v)
//=============================================================================
{
    Reg::val(U1CNFG1, v);
}

//U1EP0-15
//=============================================================================
    void        Usb::epcontrol          (uint8_t n, EPCTRL e, bool tf)
//=============================================================================
{
    n and_eq 15;
    Reg::setbit(U1EP0+(n * U1EP_SPACING), e, tf);
}

//=============================================================================
    bool        Usb::epcontrol          (uint8_t n, EPCTRL e)
//=============================================================================
{
    n and_eq 15;
    return Reg::anybit(U1EP0+(n * U1EP_SPACING), e);
}

//=============================================================================
    void        Usb::epcontrol          (uint8_t n, uint8_t v)
//=============================================================================
{
    n and_eq 15;
    Reg::val(U1EP0+(n * U1EP_SPACING), v);
}

//=============================================================================
    uint8_t     Usb::epcontrol          (uint8_t n)
//=============================================================================
{
    n and_eq 15;
    return Reg::val8(U1EP0+(n * U1EP_SPACING));
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
