#include "Irq.hpp"

enum {
INTCON = 0xBF80F000,
    TPC_SHIFT = 8, TPC_CLR = 7,
    INT4EP = 4,
    INT3EP = 3,
    INT2EP = 2,
    INT1EP = 1,
    INT0EP = 0,
PRISS = 0xBF80F010,
INTSTAT = 0xBF80F020,
IPTMR = 0xBF80F030,
IFS_BASE = 0xBF80F040,
IEC_BASE = 0xBF80F0C0,
IPC_BASE = 0xBF80F140
};

// vector function pointer storage
//=============================================================================
Irq::isrfunc_t Irq::m_isrfuncs[Irq::DMA3+1] = {0};

//=============================================================================
            auto Irq::
global      (bool tf) -> void
            {
            if(tf) __builtin_enable_interrupts();
            else __builtin_disable_interrupts();
            }

//=============================================================================
            auto Irq::
global      () -> bool
            {
            return (__builtin_mfc0(12, 0) bitand 1);
            }

//proxtimer(0) to turn off
//=============================================================================
            auto Irq::
proxtimer   (uint8_t n, uint32_t v) -> void
            { //n = priority 1-7 (and lower) starts prox timer, 0 = off
            clrbit(INTCON, TPC_CLR<<TPC_SHIFT);
            setbit(INTCON, (n bitand TPC_CLR)<<TPC_SHIFT);
            val(IPTMR, v); //timer value
            }

//=============================================================================
            auto Irq::
eint4_pol   (EINTXPOL e) -> void
            {
            setbit(INTCON, 1<<INT4EP, e);
            }

//=============================================================================
            auto Irq::
eint3_pol   (EINTXPOL e) -> void
            {
            setbit(INTCON, 1<<INT3EP, e);
            }

//=============================================================================
            auto Irq::
eint2_pol   (EINTXPOL e) -> void
            {
            setbit(INTCON, 1<<INT2EP, e);
            }

//=============================================================================
            auto Irq::
eint1_pol   (EINTXPOL e) -> void
            {
            setbit(INTCON, 1<<INT1EP, e);
            }

//=============================================================================
            auto Irq::
eint0_pol   (EINTXPOL e) -> void
            {
            setbit(INTCON, 1<<INT0EP, e);
            }

//note- the following offsets calculated in bytes as the register
//numbers are enums (not uint32_t*)- so need to calculate bytes
//the set/clr will reinterpret the enums to volatile uint32_t*
//
//vector 40 example
//reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1
//bit offset = 1<<(40%32) = 1<<8
//bit offset 8 in IFS1
//=============================================================================
            auto Irq::
flag_clr    (IRQ_VN e) -> void
            {
            clrbit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//=============================================================================
            auto Irq::
flag        (IRQ_VN e) -> bool
            {
            return anybit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

// enable/disable irq, cannot enable if no isr funtion set
//=============================================================================
            auto Irq::
on          (IRQ_VN e, bool tf) -> void
            {
            setbit(IEC_BASE + ((e / 32) * 16), 1u<<(e % 32),
                tf and m_isrfuncs[e] );
            }

//vector 17 example
//priority_shift = 8*(17%4) =  8*1= 8
//reg = 0xBF80F140 + (17/4)*16 = 0xBF80F180 = IPC4
//priority bits at bit offset 8 in IPC4
//
//init specific irq
//pri is masked to 0-7, sub is masked to 0-3
//pri of 0 disables irq
//=============================================================================
            auto Irq::
init        (IRQ_VN e, uint8_t pri, uint8_t sub, bool tf) -> void
            {
            uint32_t priority_shift = 8 * (e % 4);
            pri and_eq 7; sub and_eq 3; pri <<= 2; pri or_eq sub;
            clrbit(IPC_BASE + ((e / 4) * 16), (31<<priority_shift));
            setbit(IPC_BASE + ((e / 4) * 16), (pri<<priority_shift));
            flag_clr(e);
            on(e, tf);
            }

//init list (array) of irq's
//=============================================================================
            auto Irq::
init        (irq_list_t* arr, uint8_t sz) -> void
            {
            for(; sz; arr++, sz--) init(arr->irqvn, arr->p, arr->s, arr->en);
            }

//priority shadow set (there is only 1 extra in pic32mm)
//set specified priority (1-7) to use shadow register set
//priority values of 0 will disable shadow register set
// PRISS1<3:0> = PRISS<7:4>
// PRISS2<3:0> = PRISS<11:8>
// ...
// PRISS7<3:0> = PRISS<31:28>
// pri = 1 = 0x00000010 (1<<4) = 1<<(pri*4)
// pri = 2 = 0x00000100 (1<<8) = 1<<(pri*4)
// pri = 7 = 0x10000000 (1<<28)= 1<<(pri*4)
// pri = 0 = 0, but 1<<(pri*4) = 1, which sets SS0 (single vector shadow set)
//   is harmless in MVEC mode- will mask off anyway
//=============================================================================
            auto Irq::
shadow_set  (uint8_t pri) -> void
            { //p as uint32_t makes sure val uses 32bit version of val template
            uint32_t p = ((pri and_eq 7)*4) bitand 0xFFFFFFFE;
            val(PRISS, p);
            }


//=============================================================================
// ISR functions
//=============================================================================


// set isr function (or unset)
// if unset, make sure irq is off
//=============================================================================
            auto Irq::
isr_func    (IRQ_VN n, isrfunc_t f) -> void
            {
            m_isrfuncs[n] = f;
            if(not f) on(n, false);
            }

// run isr function
// get vector number from SIRQ, run function pointer if not 0,
// clear flag after function is run
// in case no function pointer, turn off irq (should not happen, but...)
//=============================================================================
            auto Irq::
isr         () -> void
            {
            uint8_t vn = val8(INTSTAT); //SIRQ<7:0> = INTSTAT<7:0>
            isrfunc_t f = m_isrfuncs[ vn ];
            if( f ){
                f();
                flag_clr( (IRQ_VN)vn );
            } else {
                on((IRQ_VN)vn, false);
            }
            }

// using _DefaultInterrupt for all interrupts- already a weak function used
// in all vector locations, we just override it here
// function pointers used to run an isr function
// can still use priority levels, shadow set, same as using an
// isr function for each interrupt except there is only one isr function
// (isr will auto select shadow set, set ipl level)
//
// cannot use single vector mode without losing the ability to use SRS
// while still using priority levels, also cannot use multi-vector with a
// vector spacing of 0 as multi vector does not work unless spacing is >0
//
// the biggest downside is a waste of ~800 bytes as each vector takes 8 bytes
// and they all end up in the same place, also the isr function table is ~400
// bytes of ram
//
// the advantage is the following code is all we need to get the isr's
// working- we simply register an isr function for a vector through a call
// to 'Irq::isr_func()', where the passed function can be a lambda if wanted
// isr flags will also be cleared automatically
// the function pointer can be changed so can have different functions for
// a vector when wanted
//
// isr functions will have to be registered before an irq is enabled, although
// the isr() function will check for a non-zero function pointer and disable
// the irq if there is no function pointer set for the vector
//=============================================================================
            extern "C"
            auto __attribute__((interrupt))
_DefaultInterrupt (void) -> void
            {
            Irq::isr();
            }