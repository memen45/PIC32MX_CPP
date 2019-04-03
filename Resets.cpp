#include "Resets.hpp"
#include "Irq.hpp"
#include "Sys.hpp"

enum : uint32_t {
RCON = 0xBF80F600,
    CMR = 9,
    VREGS = 8,
RSWRST = 0xBF80F610,
    SWRST = 1
};

//-----------------------------------------------------------------private-----
//save rcon on boot (only one time)
static uint32_t reset_flags;

//RCON
//save flags, clear flags, return reset cause
//=============================================================================
            auto Resets::
cause       () -> CAUSE
            {
            //save flags only if not already saved, or if RCON is not 0
            //reset_flags var will be 0 on any reset as c runtime will clear
            //before this function can run
            //and if RCON is not 0, then something changed (as we clear RCON)
            if(reset_flags == 0 || val(RCON) ){
                reset_flags = val(RCON);  //save
                val(RCON, 0);    //then clear all flags (for next reset)
            }
            //check for por first- specific combo
            if(reset_flags == (BOR | POR)){
                return POR;
            }
            //then go through flags high bits to low bits
            //(sleep is before idle, so will not get false flag
            // because sleep also has idle set)
            uint8_t ret = EXTR;
            for(; ret > POR; ret >>= 1){
                if(reset_flags bitand ret) break;
            }
            return (CAUSE)ret;
            }

//check if config bits error
//=============================================================================
            auto Resets::
config_err  () -> bool
            {
            return reset_flags bitand (1<<CMR);
            }

//RSWRST
//resets after read
//=============================================================================
            auto Resets::
swreset     () -> void
            {
            Irq::global(false);
            Sys::unlock();
            setbit(RSWRST, 1<<SWRST);
            val(RSWRST);
            for(;;);
            }


//RCON
//=============================================================================
            auto Resets::
vregs       (bool tf) -> void
            {
            Sys::unlock();
            setbit(RCON, 1<<VREGS, tf);
            Sys::lock();
            }
