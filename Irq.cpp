#include "Irq.hpp"
#include "Reg.hpp"

#include "Irq_isr.hpp"

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
    IFS_BASE = 0xBF881030,
    IEC_BASE = 0xBF881060,
    IPC_BASE = 0xBF881090
};

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
            Reg::clrbit(INTCON, TPC_CLR<<TPC_SHIFT);
            Reg::setbit(INTCON, (n bitand TPC_CLR)<<TPC_SHIFT);
            }

//=============================================================================
            auto Irq::
eint4_pol   (EINTXPOL e) -> void
            {
            Reg::setbit(INTCON, 1<<INT4EP, e);
            }

//=============================================================================
            auto Irq::
eint3_pol   (EINTXPOL e) -> void
            {
            Reg::setbit(INTCON, 1<<INT3EP, e);
            }

//=============================================================================
            auto Irq::
eint2_pol   (EINTXPOL e) -> void
            {
            Reg::setbit(INTCON, 1<<INT2EP, e);
            }

//=============================================================================
            auto Irq::
eint1_pol   (EINTXPOL e) -> void
            {
            Reg::setbit(INTCON, 1<<INT1EP, e);
            }

//=============================================================================
            auto Irq::
eint0_pol   (EINTXPOL e) -> void
            {
            Reg::setbit(INTCON, 1<<INT0EP, e);
            }

//note- the following offsets calculated in bytes as the register
//numbers are enums (not uint32_t*)- so need to calculate bytes
//the Reg::set/clr will reinterpret the enums to volatile uint32_t*
//
//vector 40 example												IRQ number: 66
//reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1				+(66/32)*16 = +32
//bit offset = 1<<(40%32) = 1<<8								1<<(66%32) = 1<<2
//bit offset 8 in IFS1											offset 2 in IFS2
//=============================================================================
            auto Irq::
flag_clr    (IRQ_NR e) -> void
            {
            Reg::clrbit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//=============================================================================
            auto Irq::
flag        (IRQ_NR e) -> bool
            {
            return Reg::anybit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//=============================================================================
            auto Irq::
on          (IRQ_NR e, bool tf) -> void
            {
            Reg::setbit(IEC_BASE + ((e / 32) * 16), 1u<<(e % 32), tf);
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
            Reg::clrbit(IPC_BASE + ((vn / 4) * 16), (31<<priority_shift));
            Reg::setbit(IPC_BASE + ((vn / 4) * 16), (pri<<priority_shift));
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

/* Note that on the PIC32MX795F512L and alike not every isr flag has its own 
 * vector. Only one callback function can exist for one vector, last is used, 
 * so make sure a callback function deals with all possible isr causes for 
 * that vector */
//=============================================================================
            auto Irq::
isr_fun     (IRQ_NR e, std::function<void()> callback) -> void
            {
            isr_callback[m_lookup_vn[e]] = callback;
            }

//enables multi vectored interrupts
//=============================================================================
            auto Irq::
enable_mvec (MVEC_MODE m) -> void
            {
            Reg::setbit(INTCON, 1 << M_VEC, m);
            }
