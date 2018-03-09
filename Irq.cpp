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
    void        Irq::global             (bool tf)
//=============================================================================
{
    if(tf) __builtin_enable_interrupts();
    else __builtin_disable_interrupts();
}

//=============================================================================
    bool        Irq::global             ()
//=============================================================================
{
    return (__builtin_mfc0(12, 0) bitand 1);
}

//proxtimer(0) to turn off
//=============================================================================
    void        Irq::proxtimer          (uint8_t n, uint32_t v)
//=============================================================================
{ //n = priority 1-7 (and lower) starts prox timer, 0 = prox timer off
    Reg::clrbit(INTCON, TPC_CLR<<TPC_SHIFT);
    Reg::setbit(INTCON, (n bitand TPC_CLR)<<TPC_SHIFT);
    Reg::val(IPTMR, v); //timer value
}


//=============================================================================
    void        Irq::eint4_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT4EP, e);
}

//=============================================================================
    void        Irq::eint3_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT3EP, e);
}

//=============================================================================
    void        Irq::eint2_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT2EP, e);
}

//=============================================================================
    void        Irq::eint1_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT1EP, e);
}

//=============================================================================
    void        Irq::eint0_pol          (EINTXPOL e)
//=============================================================================
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
    void        Irq::flag_clr           (IRQ_VN e)
//=============================================================================
{
    Reg::clrbit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
}

//=============================================================================
    bool        Irq::flag               (IRQ_VN e)
//=============================================================================
{
    return Reg::anybit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
}

//=============================================================================
    void        Irq::on                 (IRQ_VN e, bool tf)
//=============================================================================
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
    void        Irq::init       (IRQ_VN e, uint8_t pri, uint8_t sub, bool tf)
//=============================================================================
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
    void        Irq::init               (irq_list_t* arr)
//=============================================================================
{
    for(; arr->irqvn < END; arr++){
        init(arr->irqvn, arr->p, arr->s, arr->en);
    }
}

//priority shadow set, 0 or 1
//pri = 1-7, tf =0,1 (0=normal, 1=shadow)
//pri is masked to 0-7
//pri val of 0 will set/clr SS0- no harm as not using (single vector ss)
//=============================================================================
    void        Irq::shadow_set         (uint8_t pri, bool tf)
//=============================================================================
{
    pri and_eq 7; pri <<= 2; //0*4=0 1*4=4, 7*4=28
    Reg::setbit(PRISS, 1<<pri, tf);
}
