#pragma once

#include "Irq.hpp"
#include "Reg.hpp"
#include "Dma.hpp"

class Sys {

    public:

    //cfgcon
    enum BMXARB : uint8_t { CPUHIGH = 0, CPULOW, RROBIN };

    static void     bus_err     (bool);
    static void     bus_mode    (BMXARB);
    static void     ram_exec    (uint8_t);
    static void     jtag        (bool);

    //devid
    static uint32_t devid       ();
    static uint8_t  ver         ();

    //syskey
    static void     lock        ();
    static void     unlock      ();

    //ancfg
    static void     bgap_adc    (bool);
    static void     bgap_comp   (bool);

    //udid
    static uint32_t udid        (uint8_t); //0-4 ->UDID1-5

    //misc
    //static uint32_t flash_size  ();
    //static uint32_t ram_size    ();

    private:

    static Reg r;
    static Irq ir;

    enum {
        CFGCON = 0xBF803640,
            BMXERRDIS = 1<<27,
            BMXARB_SHIFT = 24, BMXARB_CLR = 3,
            EXECADDR_SHIFT = 16, EXECADDR_CLR = 255,
            JTAGEN = 1<<3,
        DEVID = 0xBF803660,
            VER_SHIFT = 28, VER_CLR = 15,
            ID_SHIFT = 0, ID_CLR = 0xFFFFFFF,
        SYSKEY = 0xBF803670,
            MAGIC1 = 0xAA996655,
            MAGIC2 = 0x556699AA,
        ANCFG = 0xBF802300,
            VBGADC = 1<<2,
            VBGCMP = 1<<1,
        UDID1 = 0xBF801840,
        UDID2 = 0xBF801850,
        UDID3 = 0xBF801860,
        UDID4 = 0xBF801870,
        UDID5 = 0xBF801880,

    };

};

//cfgcon
//=============================================================================
    void        Sys::bus_err            (bool tf)
//=============================================================================
{
    r.setbit(CFGCON, BMXERRDIS, !tf);
}

//=============================================================================
    void        Sys::bus_mode           (BMXARB e)
//=============================================================================
{
    r.clrbit(CFGCON, BMXARB_CLR<<BMXARB_SHIFT);
    r.setbit(CFGCON, e<<BMXARB_SHIFT);
}

//=============================================================================
    void        Sys::ram_exec           (uint8_t v)
//=============================================================================
{
    r.val(CFGCON+2, v);
}

//=============================================================================
    void        Sys::jtag               (bool tf)
//=============================================================================
{
    r.setbit(CFGCON, JTAGEN, tf);
}

//devid
//=============================================================================
    uint32_t    Sys::devid              ()
//=============================================================================
{
    return r.val(DEVID) & ID_CLR;
}

//=============================================================================
    uint8_t     Sys::ver                ()
//=============================================================================
{
    return r.val(DEVID) >> VER_SHIFT;
}

//ancfg
//=============================================================================
    void        Sys::bgap_adc           (bool tf)
//=============================================================================
{
    r.setbit(ANCFG, VBGADC, tf);
}

//=============================================================================
    void        Sys::bgap_comp          (bool tf)
//=============================================================================
{
    r.setbit(ANCFG, VBGCMP, tf);
}

//udid
//=============================================================================
    uint32_t    Sys::udid               (uint8_t v)
//=============================================================================
{
    if(v > 4) v = 4;
    return r.val(UDID1+v);
}

//misc
//uint32_t Sys::flash_size(){
//    return 32768 << ((devid() >> 15) & 3);
//}

//uint32_t Sys::ram_size(){
//    return flash_size() == 0x40000 ? 32768 : 16384;
//}

// PIC32MM0064GPM028 0x07708053 0b0000 0111 0111 0000 1000 0000 0101 0011 64/16
// PIC32MM0128GPM028 0x07710053 0b0000 0111 0111 0001 0000 0000 0101 0011 128/16
// PIC32MM0256GPM028 0x07718053 0b0000 0111 0111 0001 1000 0000 0101 0011 256/32

// PIC32MM0064GPM036 0x0770A053 0b0000 0111 0111 0000 1010 0000 0101 0011
// PIC32MM0128GPM036 0x07712053 0b0000 0111 0111 0001 0010 0000 0101 0011
// PIC32MM0256GPM036 0x0771A053 0b0000 0111 0111 0001 1010 0000 0101 0011

// PIC32MM0064GPM048 0x0772C053 0b0000 0111 0111 0010 1100 0000 0101 0011
// PIC32MM0128GPM048 0x07734053 0b0000 0111 0111 0011 0100 0000 0101 0011
// PIC32MM0256GPM048 0x0773C053 0b0000 0111 0111 0011 1100 0000 0101 0011

// PIC32MM0064GPM064 0x0770E053 0b0000 0111 0111 0000 1110 0000 0101 0011
// PIC32MM0128GPM064 0x07716053 0b0000 0111 0111 0001 0110 0000 0101 0011
// PIC32MM0256GPM064 0x0771E053 0b0000 0111 0111 0001 1110 0000 0101 0011
