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
    VBGCMP = 1
};

//-----------------------------------------------------------------private-----
//syskey lock/unlock
//keep track of unlock count-
//inc unlock_count on lock(), dec unlock_count on unlock()
//unlock done when unlock_count is 0
//lock done when unlock_count is 0
static volatile uint8_t unlock_count;

//=============================================================================
            auto Sys::
lock        () -> void
            {
            bool irqstate = Irq::global();              //get STATUS.IE
            Irq::global(false);
            //unlock_count only accessed with irq off
            if(unlock_count) unlock_count--;            //dec counter
            if(unlock_count == 0) Reg::val(SYSKEY, 0);  //if 0, lock
            //
            if(irqstate) Irq::global(true);             //restore IE state
            }

//=============================================================================
            auto Sys::
unlock      () -> void
            {
            bool irqstate = Irq::global();              //get STATUS.IE
            Irq::global(false);
            bool dmasusp = Dma::all_suspend();          // DMA suspend status
            Dma::all_suspend(true);                     //suspend DMA
            //
            if(unlock_count == 0){                      //first time, unlock
                Reg::val(SYSKEY, MAGIC1);
                Reg::val(SYSKEY, MAGIC2);
            }
            unlock_count++;                             //inc unlock_count
            //
            if(not dmasusp) Dma::all_suspend(false);    //DMA resume
            if(irqstate) Irq::global(true);             //restore IE state
            }

//provide previous irq/dma status
//=============================================================================
            auto Sys::
lock        (uint8_t v) -> void
            {
            lock();
            Dma::all_suspend(v & 2);                    //restore dma
            if(v & 1) Irq::global(true);                //restore irq
            }

//blocking, return irq/dma status for use in next call to lock()
//=============================================================================
            auto Sys::
unlock_wait () -> uint8_t
            {
            bool irqstate = Irq::global();              //get STATUS.IE
            Irq::global(false);
            bool dmasusp = Dma::all_suspend();          // DMA suspend status
            Dma::all_suspend(true);                     //suspend DMA
            //
            if(unlock_count == 0){                      //first time, unlock
                Reg::val(SYSKEY, MAGIC1);
                Reg::val(SYSKEY, MAGIC2);
            }
            unlock_count++;                             //inc unlock_count
            //
            return (dmasusp<<1) bitor irqstate;
            }

//cfgcon
//=============================================================================
            auto Sys::
bus_err     (bool tf) -> void
            {
            Reg::setbit(CFGCON, 1<<BMXERRDIS, !tf);
            }

//=============================================================================
            auto Sys::
bus_mode    (BMXARB e) -> void
            {
            Reg::clrbit(CFGCON, BMXARB_MASK<<BMXARB_SHIFT);
            Reg::setbit(CFGCON, e<<BMXARB_SHIFT);
            }

//=============================================================================
            auto Sys::
ram_exec    (uint8_t v) -> void
            {
            Reg::val(CFGCON+2, v);
            }

//=============================================================================
            auto Sys::
jtag        (bool tf) -> void
            {
            Reg::setbit(CFGCON, 1<<JTAGEN, tf);
            }

//devid
//=============================================================================
            auto Sys::
devid       () -> uint32_t
            {
            return Reg::val(DEVID) bitand ID_MASK;
            }

//=============================================================================
            auto Sys::
ver         () -> uint8_t
            {
            return Reg::val(DEVID)>>VER_SHIFT;
            }

//ancfg
//=============================================================================
            auto Sys::
bgap_adc    (bool tf) -> void
            {
            Reg::setbit(ANCFG, 1<<VBGADC, tf);
            }

//=============================================================================
            auto Sys::
bgap_comp   (bool tf) -> void
            {
            Reg::setbit(ANCFG, 1<<VBGCMP, tf);
            }

//udid
//=============================================================================
            auto Sys::
udid        (UDID e) -> uint64_t
            {
            union { uint8_t b[8]; uint16_t h[4]; uint32_t w[2]; uint64_t ww; }
                u = { 0 };
            if(e == LOTSCRIBE){ //combine LOT-H|LOT-L|SCRIBE
                u.w[1] = Reg::val(e) & 0x00FFFFFF;      //0x00hhhhhh........
                u.w[0] = Reg::val(e+4) << 8;            //0x........llllll00
                u.b[0] = Reg::val8(e+8);                //0x..............ss
            } else { //combine DIE-X|DIE-Y (only 32bits used)
                u.h[1] = Reg::val16(e);                 //0x........xxxx....
                u.h[0] = Reg::val16(e+4);               //0x............yyyy
            }
            return u.ww;
            //0x00hhhhhhllllllss - LOTSCRIBE
            //0x00000000xxxxyyyy - DIEXY
            }
