#include "Resets.hpp"
#include "Irq.hpp"
#include "Sys.hpp"
#include "Reg.hpp"

enum : uint32_t {
    RCON = 0xBF8026E0,
        PORIO = 1u<<31,
        PORCORE = 1<<30,
        BCFGERR = 1<<27,
        BCFGFAIL = 1<<26,
        CMR = 1<<9,
    RSWRST = 0xBF8026F0,
        SWRST = 1,
    RNMICON = 0xBF802700,
        WDTR = 1<<24,
        SWNMI = 1<<23,
        GNMI = 1<<19,
        CF = 1<<17,
        WDTS = 1<<16,
    PWRCON = 0xBF802710,
        SBOREN = 1<<2,
        RETEN = 1<<0,
        VREGS = 1<<0
};

//save rcon on boot (only one time)
static uint32_t boot_flags;

//RCON
//save flags, clear flags, return reset cause
//=============================================================================
    auto        Resets::cause               () -> CAUSE
//=============================================================================
{
    //save flags only if not already saved
    //boot_flags var will be 0 on any reset as c runtime will clear
    //before this function can run
    if(boot_flags == 0){
        boot_flags = Reg::val(RCON);  //save
        Reg::val(RCON, 0);            //then clear all flags (for next reset)
    }
    //check for por first- specific combo
    if(boot_flags == (PORIO | PORCORE | BOR | POR)) return POR;
    //then go through flags high bits to low bits
    //(sleep is before idle, so will not get false flag
    // because sleep also has idle set)
    uint8_t ret = EXTR;
    for(; ret > POR; ret >>= 1){
        if(boot_flags bitand ret) break;
    }
    return (CAUSE)ret;
}

//check if config bits error
//=============================================================================
    bool        Resets::config_err          ()
//=============================================================================
{
    return boot_flags bitand (BCFGERR | BCFGFAIL | CMR);
}

//RSWRST
//resets after read
//=============================================================================
    void        Resets::swreset             ()
//=============================================================================
{
    Irq::disable_all();
    Sys::unlock();
    Reg::setbit(RSWRST, SWRST);
    Reg::val(RSWRST);
    for(;;);
}

//RNMICON
//=============================================================================
    bool        Resets::nmi_wdt             ()
//=============================================================================
{
    return Reg::anybit(RNMICON, WDTR);
}

//=============================================================================
    bool        Resets::nmi_sw              ()
//=============================================================================
{
    return Reg::anybit(RNMICON, SWNMI);
}
    bool        Resets::nmi_gen             ()
//=============================================================================
{
    return Reg::anybit(RNMICON, GNMI);
}

//=============================================================================
    bool        Resets::nmi_clkf            ()
//=============================================================================
{
    return Reg::anybit(RNMICON, CF);
}

//=============================================================================
    bool        Resets::nmi_wdts            ()
//=============================================================================
{
    return Reg::anybit(RNMICON, WDTS);
}

//=============================================================================
    void        Resets::nmi_wdtcount        (uint16_t v)
//=============================================================================
{
    Reg::val(RNMICON, v);
}

//=============================================================================
    void        Resets::nmi_wdtclr          ()
//=============================================================================
{
    Reg::clrbit(RNMICON, WDTR);
}

//PWRCON
//=============================================================================
    void        Resets::bor                 (bool tf)
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(PWRCON, SBOREN, tf);
    Sys::lock();
}

//=============================================================================
    void        Resets::reten               (bool tf)
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(PWRCON, RETEN, tf);
    Sys::lock();
}

//=============================================================================
    void        Resets::vregs               (bool tf)
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(PWRCON, VREGS, tf);
    Sys::lock();
}
