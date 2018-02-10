#include "Resets.hpp"
#include "Irq.hpp"

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
        boot_flags = r.val(RCON);  //save
        r.val(RCON, 0);            //then clear all flags (for next reset)
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
    sys.unlock();
    r.setbit(RSWRST, SWRST);
    r.val(RSWRST);
    for(;;);
}

//RNMICON
//=============================================================================
    bool        Resets::nmi_wdt             ()
//=============================================================================
{
    return r.anybit(RNMICON, WDTR);
}

//=============================================================================
    bool        Resets::nmi_sw              ()
//=============================================================================
{
    return r.anybit(RNMICON, SWNMI);
}
    bool        Resets::nmi_gen             ()
//=============================================================================
{
    return r.anybit(RNMICON, GNMI);
}

//=============================================================================
    bool        Resets::nmi_clkf            ()
//=============================================================================
{
    return r.anybit(RNMICON, CF);
}

//=============================================================================
    bool        Resets::nmi_wdts            ()
//=============================================================================
{
    return r.anybit(RNMICON, WDTS);
}

//=============================================================================
    void        Resets::nmi_wdtcount        (uint16_t v)
//=============================================================================
{
    r.val(RNMICON, v);
}

//=============================================================================
    void        Resets::nmi_wdtclr          ()
//=============================================================================
{
    r.clrbit(RNMICON, WDTR);
}

//PWRCON
//=============================================================================
    void        Resets::bor                 (bool tf)
//=============================================================================
{
    sys.unlock();
    r.setbit(PWRCON, SBOREN, tf);
    sys.lock();
}

//=============================================================================
    void        Resets::reten               (bool tf)
//=============================================================================
{
    sys.unlock();
    r.setbit(PWRCON, RETEN, tf);
    sys.lock();
}

//=============================================================================
    void        Resets::vregs               (bool tf)
//=============================================================================
{
    sys.unlock();
    r.setbit(PWRCON, VREGS, tf);
    sys.lock();
}
