#include "Sys.hpp"
#include "Irq.hpp"
#include "Reg.hpp"
#include "Dma.hpp"

enum {
    BMXCON = 0xBF882000,
        BMXERRDIS = 27,
        BMXARB_SHIFT = 0, BMXARB_MASK = 7,
	BMXDKPBA = 0xBF882010,										//Kernel Program (RAM)
	BMXDUDBA = 0xBF882020,										//User Data Base Address (RAM)
	BMXDUPBA = 0xBF882030,										//User Program (RAM)
	BMXPUPBA = 0xBF882050,										//User Program (Program Flash)
        //EXECADDR_SHIFT = 16, EXECADDR_MASK = 255,
	DDPCON = 0xBF80F200,
        JTAGEN = 3,
    DEVID = 0xBF80F220,
        VER_SHIFT = 28, VER_MASK = 15,
        ID_SHIFT = 0, ID_MASK = 0xFFFFFFF,
    SYSKEY = 0xBF803670,
        MAGIC1 = 0xAA996655,
        MAGIC2 = 0x556699AA,
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
    bool irqstate = Irq::all_ison();                //get STATUS.IE
    Irq::disable_all();
    //unlock_count only accessed with irq off
    if(unlock_count) unlock_count--;                //dec counter
    if(unlock_count == 0) Reg::val(SYSKEY, 0);      //if 0, lock
    //
    if(irqstate) Irq::enable_all();                 //restore IE state
}

//=============================================================================
    void        Sys::unlock         ()
//=============================================================================
{
    bool irqstate = Irq::all_ison();                //get STATUS.IE
    Irq::disable_all();
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
    if(irqstate) Irq::enable_all();                 //restore IE state
}

//provide previous irq/dma status
//=============================================================================
    void        Sys::lock           (uint8_t v)
//=============================================================================
{
    Reg::val(SYSKEY, 0);                            //lock
    Dma::all_suspend(v & 2);                        //restore dma
    if(v & 1) Irq::enable_all();                    //restore irq
}

//blocking, return irq/dma status for use in next call to lock()
//=============================================================================
    uint8_t     Sys::unlock_wait    ()
//=============================================================================
{
    bool irqstate = Irq::all_ison();                //get STATUS.IE
    Irq::disable_all();
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
    Reg::setbit(BMXCON, 1<<BMXERRDIS, !tf);
}

//=============================================================================
    void        Sys::bus_mode           (BMXARB e)
//=============================================================================
{
    Reg::clrbit(BMXCON, BMXARB_MASK<<BMXARB_SHIFT);
    Reg::setbit(BMXCON, e<<BMXARB_SHIFT);
}

//=============================================================================
    void        Sys::jtag               (bool tf)
//=============================================================================
{
    Reg::setbit(DDPCON, 1<<JTAGEN, tf);
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
