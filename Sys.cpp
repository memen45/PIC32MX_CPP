#include "Sys.hpp"
#include "Irq.hpp"
#include "Reg.hpp"
#include "Dma.hpp"

enum {
    CFGCON = 0xBF803640,
        BMXERRDIS = 27,
        BMXARB_SHIFT = 24, BMXARB_MASK = 3,
        EXECADDR_SHIFT = 16, EXECADDR_MASK = 255,
        JTAGEN = 3,
    DEVID = 0xBF803660,
        VER_SHIFT = 28, VER_MASK = 15,
        ID_SHIFT = 0, ID_MASK = 0xFFFFFFF,
    SYSKEY = 0xBF803670,
        MAGIC1 = 0xAA996655,
        MAGIC2 = 0x556699AA,
    ANCFG = 0xBF802300,
        VBGADC = 2,
        VBGCMP = 1,
    UDID1 = 0xBF801840,
    UDID2 = 0xBF801850,
    UDID3 = 0xBF801860,
    UDID4 = 0xBF801870,
    UDID5 = 0xBF801880,

};


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
    bool irqstate = Irq::global();                  //get STATUS.IE
    Irq::global(false);
    //unlock_count only accessed with irq off
    if(unlock_count) unlock_count--;                //dec counter
    if(unlock_count == 0) Reg::val(SYSKEY, 0);      //if 0, lock
    //
    if(irqstate) Irq::global(true);                 //restore IE state
}

//=============================================================================
    void        Sys::unlock         ()
//=============================================================================
{
    bool irqstate = Irq::global();                  //get STATUS.IE
    Irq::global(false);
    bool dmasusp = Dma::all_suspend();              //get DMA suspend status
    Dma::all_suspend(true);                         //suspend DMA
    //
    if(unlock_count == 0){                          //first time, unlock
        Reg::val(SYSKEY, MAGIC1);
        Reg::val(SYSKEY, MAGIC2);
    }
    unlock_count++;                                 //inc unlock_count
    //
    if(not dmasusp) Dma::all_suspend(false);        //DMA resume
    if(irqstate) Irq::global(true);                 //restore IE state
}

//provide previous irq/dma status
//=============================================================================
    void        Sys::lock           (uint8_t v)
//=============================================================================
{
    Reg::val(SYSKEY, 0);                            //lock
    Dma::all_suspend(v & 2);                        //restore dma
    if(v & 1) Irq::global(true);                    //restore irq
}

//blocking, return irq/dma status for use in next call to lock()
//=============================================================================
    uint8_t     Sys::unlock_wait    ()
//=============================================================================
{
    bool irqstate = Irq::global();                  //get STATUS.IE
    Irq::global(false);
    bool dmasusp = Dma::all_suspend();              //get DMA suspend status
    Dma::all_suspend(true);                         //suspend DMA
    Reg::val(SYSKEY, MAGIC1);
    Reg::val(SYSKEY, MAGIC2);
    return (dmasusp<<1) bitor irqstate;
}

//cfgcon
//=============================================================================
    void        Sys::bus_err            (bool tf)
//=============================================================================
{
    Reg::setbit(CFGCON, 1<<BMXERRDIS, !tf);
}

//=============================================================================
    void        Sys::bus_mode           (BMXARB e)
//=============================================================================
{
    Reg::clrbit(CFGCON, BMXARB_MASK<<BMXARB_SHIFT);
    Reg::setbit(CFGCON, e<<BMXARB_SHIFT);
}

//=============================================================================
    void        Sys::ram_exec           (uint8_t v)
//=============================================================================
{
    Reg::val(CFGCON+2, v);
}

//=============================================================================
    void        Sys::jtag               (bool tf)
//=============================================================================
{
    Reg::setbit(CFGCON, 1<<JTAGEN, tf);
}

//devid
//=============================================================================
    uint32_t    Sys::devid              ()
//=============================================================================
{
    return Reg::val(DEVID) bitand ID_MASK;
}

//=============================================================================
    uint8_t     Sys::ver                ()
//=============================================================================
{
    return Reg::val(DEVID)>>VER_SHIFT;
}

//ancfg
//=============================================================================
    void        Sys::bgap_adc           (bool tf)
//=============================================================================
{
    Reg::setbit(ANCFG, 1<<VBGADC, tf);
}

//=============================================================================
    void        Sys::bgap_comp          (bool tf)
//=============================================================================
{
    Reg::setbit(ANCFG, 1<<VBGCMP, tf);
}

//udid
//=============================================================================
    uint32_t    Sys::udid               (uint8_t v)
//=============================================================================
{
    if(v > 4) v = 4;
    return Reg::val(UDID1 + v);
}
