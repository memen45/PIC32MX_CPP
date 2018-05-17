#include "Irq.hpp"
#include "Reg.hpp"

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
            Reg::val(IPTMR, v); //timer value
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
//vector 40 example
//reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1
//bit offset = 1<<(40%32) = 1<<8
//bit offset 8 in IFS1
//=============================================================================
            auto Irq::
flag_clr    (IRQ_VN e) -> void
            {
            Reg::clrbit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//=============================================================================
            auto Irq::
flag        (IRQ_VN e) -> bool
            {
            return Reg::anybit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
            }

//=============================================================================
            auto Irq::
on          (IRQ_VN e, bool tf) -> void
            {
            Reg::setbit(IEC_BASE + ((e / 32) * 16), 1u<<(e % 32), tf);
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
            Reg::clrbit(IPC_BASE + ((e / 4) * 16), (31<<priority_shift));
            Reg::setbit(IPC_BASE + ((e / 4) * 16), (pri<<priority_shift));
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
//=============================================================================
            auto Irq::
shadow_set  (uint8_t pri) -> void
            { //p as uint32_t makes sure Reg::val uses 32bit version template
            uint32_t p = (pri and_eq 7)*4;
            Reg::val(PRISS, p ? 1<<p : 0); //0=all clear, else single bit set
            }
