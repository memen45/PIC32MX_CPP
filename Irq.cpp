#include "Irq.hpp"

enum {
    INTCON = 0xBF881000,
        M_VEC = 12,
        TPC_SHIFT = 8, TPC_CLR = 7,
        INT4EP = 4,
        INT3EP = 3,
        INT2EP = 2,
        INT1EP = 1,
        INT0EP = 0,
    INTSTAT = 0xBF881010,
    IPTMR = 0xBF881020,
    IFS_BASE = 0xBF881030,
    IEC_BASE = 0xBF881060,
    IPC_BASE = 0xBF881090
};

// vector function pointer storage
//=============================================================================
Irq::isrfunc_t Irq::m_isrfuncs[Irq::UART5_TX + 1] = {0};

// vector number to irq number lookup table
constexpr uint8_t Irq::m_lookup_vn[];

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
//vector 40 example												IRQ number: 66
//reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1				+(66/32)*16 = +32
//bit offset = 1<<(40%32) = 1<<8								1<<(66%32) = 1<<2
//bit offset 8 in IFS1											offset 2 in IFS2
//=============================================================================
            auto Irq::
flag_clr    (IRQ_NR e) -> void
            {
            clrbit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//=============================================================================
            auto Irq::
flag        (IRQ_NR e) -> bool
            {
            return anybit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

// enable/disable irq, cannot enable if no isr funtion set
//=============================================================================
            auto Irq::
on          (IRQ_NR e, bool tf) -> void
            {
            setbit(IEC_BASE + ((e / 32) * 16), 1u<<(e % 32),
                tf and m_isrfuncs[m_lookup_vn[e]] );
            }
//=============================================================================
            auto Irq::
on          (IRQ_NR e) -> bool
            {
            return anybit(IEC_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//vector 17 example												Vector number: 28
//priority_shift = 8*(17%4) =  8*1= 8							8*(28%4) = 8*2 = 0
//reg = 0xBF80F140 + (17/4)*16 = 0xBF80F180 = IPC4				+ (28/4)*16 = +112
//priority bits at bit offset 8 in IPC4							offset 0 in IPC7
//
//init specific irq
//pri is masked to 0-7, sub is masked to 0-3
//pri of 0 disables irq
//=============================================================================
            auto Irq::
init        (IRQ_NR e, uint8_t pri, uint8_t sub, bool tf) -> void
            {
            uint8_t vn = m_lookup_vn[e];
            uint32_t priority_shift = 8 * (vn % 4);
            pri and_eq 7; sub and_eq 3; pri <<= 2; pri or_eq sub;
            clrbit(IPC_BASE + ((vn / 4) * 16), (31<<priority_shift));
            setbit(IPC_BASE + ((vn / 4) * 16), (pri<<priority_shift));
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

//enables multi vectored interrupts
//=============================================================================
            auto Irq::
enable_mvec (MVEC_MODE m) -> void
            {
            setbit(INTCON, 1 << M_VEC, m);
            }


//=============================================================================
// ISR functions
//=============================================================================


// set isr function (or unset)
// if unset, make sure irq is off
//=============================================================================
            auto Irq::
isr_func    (IRQ_NR n, isrfunc_t f) -> void
            {
            m_isrfuncs[m_lookup_vn[n]] = f;
            if(not f) on(n, false);
            }

// run isr function
// run function pointer if not 0,
// clear flag after function is run
// in case no function pointer, turn off irq (should not happen, but...)
//=============================================================================
            auto Irq::
isr         (uint8_t vn) -> void
            {
            isrfunc_t f = m_isrfuncs[ vn ];
            if( f ){
                f();
                // flag_clr( (IRQ_NR)vn );      // cannot clear flag as irq nr is ambiguous
            } else {
                on((IRQ_NR)vn, false);
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
            uint8_t vn = Reg::val8(INTSTAT); //SIRQ<7:0> = INTSTAT<7:0>
            Irq::isr(vn);
            }
