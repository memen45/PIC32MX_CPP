#include "Sys.hpp"

//syskey lock/unlock
//keep track of unlock count-
//inc unlock_count on lock(), dec unlock_count on unlock()
//unlock done when unlock_count is 0
//lock done when unlock_count is 0
static volatile uint8_t unlock_count;

//=============================================================================
    void        Sys::lock           ()
//=============================================================================
{
    bool irqstate = Irq::all_ison();                //get STATUS.IE
    ir.disable_all();
    //unlock_count only accessed with irq off
    if(unlock_count) unlock_count--;                //dec counter
    if(unlock_count == 0) r.val(SYSKEY, 0);         //if 0, lock
    //
    if(irqstate) ir.enable_all();                   //restore IE state
}

//=============================================================================
    void        Sys::unlock         ()
//=============================================================================
{
    bool irqstate = ir.all_ison();                  //get STATUS.IE
    ir.disable_all();
    bool dmasusp = Dma::all_suspend();              //get DMA suspend status
    Dma::all_suspend(true);                         //suspend DMA
    //
    if(unlock_count == 0){                          //first time, unlock
        r.val(SYSKEY, MAGIC1);
        r.val(SYSKEY, MAGIC2);
    }
    unlock_count++;                                 //inc unlock_count
    //
    if(not dmasusp) Dma::all_suspend(false);        //DMA resume
    if(irqstate) ir.enable_all();                   //restore IE state
}

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
    return r.val(DEVID) bitand ID_CLR;
}

//=============================================================================
    uint8_t     Sys::ver                ()
//=============================================================================
{
    return r.val(DEVID)>>VER_SHIFT;
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
    return r.val(UDID1 + v);
}
