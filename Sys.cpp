#include "Sys.hpp"
#include "Irq.hpp"
#include "Dma.hpp"
#include "Osc.hpp"

enum {
    BMXCON = 0xBF882000,
        //BMXERRDIS = 27,
		BMXERRIXI = 20,
		BMXERRICD = 19,
		BMXERRDMA = 18,
		BMXERRDS = 17,
		BMXERRIS = 16,
		BMXWSDRM = 6,
        BMXARB_SHIFT = 0, BMXARB_MASK = 7,
        
	BMXDKPBA = 0xBF882010,									// Kseg Program (RAM) base address (until User Data RAM)
	BMXDUDBA = 0xBF882020,									// User Data (RAM) base Address (until  User Program RAM)
	BMXDUPBA = 0xBF882030,									// User Program (RAM) base address (until end of RAM)
	
        //EXECADDR_SHIFT = 16, EXECADDR_MASK = 255,
	
	BMXDRMSZ = 0xBF882040,									// Data Ram size
	BMXPUPBA = 0xBF882050,									// User Program (Flash) base address (until end of PFM)
	BMXPFMSZ = 0xBF882060,									// Program Flash Memory (PFM) size
	BMXBOOTSZ = 0xBF882070,									// Boot Flash size
	
    
	DDPCON = 0xBF80F200,
        JTAGEN = 3,
    DEVID = 0xBF80F220,
        VER_SHIFT = 28, VER_MASK = 15,
        ID_SHIFT = 0, ID_MASK = 0xFFFFFFF,
    SYSKEY = 0xBF80F230,
        MAGIC1 = 0xAA996655,
        MAGIC2 = 0x556699AA,
    CHECON = 0xBF884000,
        PFMWS_SHIFT = 0, PFMWS_MASK = 7,
        PREFEN_SHIFT = 4, PREFEN_MASK = 3,
		
    FLASHSPEED = 30000000
};


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
            if(unlock_count == 0) val(SYSKEY, 0);  //if 0, lock
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
                val(SYSKEY, MAGIC1);
                val(SYSKEY, MAGIC2);
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
                val(SYSKEY, MAGIC1);
                val(SYSKEY, MAGIC2);
            }
            unlock_count++;                             //inc unlock_count
            //
            return (dmasusp<<1) bitor irqstate;
            }

//cfgcon
//=============================================================================
//            auto Sys::
//bus_err     (bool tf) -> void
//            {
//            setbit(BMXCON, 1<<BMXERRDIS, !tf);
//            }

//=============================================================================
            auto Sys::
bus_mode    (BMXARB e) -> void
            {
            clrbit(BMXCON, BMXARB_MASK<<BMXARB_SHIFT);
            setbit(BMXCON, e<<BMXARB_SHIFT);
            }

//=============================================================================
            auto Sys::
jtag        (bool tf) -> void
            {
            setbit(DDPCON, 1<<JTAGEN, tf);
            }

//devid
//=============================================================================
            auto Sys::
devid       () -> uint32_t
            {
            return val(DEVID) bitand ID_MASK;
            }

//=============================================================================
            auto Sys::
ver         () -> uint8_t
            {
            return val(DEVID)>>VER_SHIFT;
            }

//=============================================================================
            auto Sys::
waitstates  (uint32_t clk) -> void
            {
            uint8_t waitstates = clk / FLASHSPEED;
            bool irqstate = Irq::global();                //get STATUS.IE
            Irq::global(false);
            clrbit(BMXCON, 1 << BMXWSDRM);				//Disable RAM wait states
            val(CHECON, waitstates << PFMWS_SHIFT);
            if(irqstate) Irq::global(true);                 //restore IE state
            }
    
//=============================================================================
			auto Sys::
flash_size	() -> uint32_t
			{
			return val(BMXPFMSZ);
			}
    
//=============================================================================
            auto Sys::
pcache      (PREFEN p) -> void
            {
            bool irqstate = Irq::global();                //get STATUS.IE
            Irq::global(false);
            clrbit(CHECON, PREFEN_MASK << PREFEN_SHIFT);
            setbit(CHECON, p << PREFEN_SHIFT);
            kseg0_cache_enable(ON);
            if(irqstate) Irq::global(true);                 //restore IE state
            }

//=============================================================================
            auto Sys::
kseg0_cache_enable  (KSEG0_CACHE m) -> void
            {
            uint32_t tmp = __builtin_mfc0(16, 0) ;
            tmp = (tmp & (~7)) | m;
            __builtin_mtc0(16, 0, tmp);
            }
