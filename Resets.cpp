#include "Resets.hpp"
#include "Irq.hpp"
#include "Sys.hpp"

enum : uint32_t {
RCON = 0xBF8026E0,
    PORIO = 31,
    PORCORE = 30,
    BCFGERR = 27,
    BCFGFAIL = 26,
    CMR = 9,
RSWRST = 0xBF8026F0,
    SWRST = 0,
RNMICON = 0xBF802700,
    WDTR = 24,
    SWNMI = 23,
    GNMI = 19,
    CF = 17,
    WDTS = 16,
PWRCON = 0xBF802710,
    SBOREN = 2,
    RETEN = 0,
    VREGS = 0
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
            if(reset_flags == ((1u<<PORIO) | (1<<PORCORE) | BOR | POR)){
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
            return reset_flags bitand (1<<BCFGERR | 1<<BCFGFAIL | 1<<CMR);
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

//RNMICON
//=============================================================================
            auto Resets::
nmi_wdt     () -> bool
            {
            return anybit(RNMICON, 1<<WDTR);
            }

//=============================================================================
            auto Resets::
nmi_sw      () -> bool
            {
            return anybit(RNMICON, 1<<SWNMI);
            }

//=============================================================================
            auto Resets::
nmi_gen     () -> bool
            {
            return anybit(RNMICON, 1<<GNMI);
            }

//=============================================================================
            auto Resets::
nmi_clkf    () -> bool
            {
            return anybit(RNMICON, 1<<CF);
            }

//=============================================================================
            auto Resets::
nmi_wdts    () -> bool
            {
            return anybit(RNMICON, 1<<WDTS);
            }

//=============================================================================
            auto Resets::
nmi_wdtcount (uint16_t v) -> void
            {
            val(RNMICON, v);
            }

//=============================================================================
            auto Resets::
nmi_wdtclr  () -> void
            {
            clrbit(RNMICON, 1<<WDTR);
            }

//PWRCON
//=============================================================================
            auto Resets::
bor         (bool tf) -> void
            {
            Sys::unlock();
            setbit(PWRCON, 1<<SBOREN, tf);
            Sys::lock();
            }

//=============================================================================
            auto Resets::
reten       (bool tf) -> void
            {
            Sys::unlock();
            setbit(PWRCON, 1<<RETEN, tf);
            Sys::lock();
            }

//=============================================================================
            auto Resets::
vregs       (bool tf) -> void
            {
            Sys::unlock();
            setbit(PWRCON, 1<<VREGS, tf);
            Sys::lock();
            }
