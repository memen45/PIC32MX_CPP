#include "Cp0.hpp"
#include "Irq.hpp"
#include "Reg.hpp"
#include "Osc.hpp"

static uint32_t m_compare_count; //save count for reloads

//=============================================================================
    uint32_t        Cp0::count              ()
//=============================================================================
{
    return __builtin_mfc0(9, 0);
}

//=============================================================================
    uint32_t        Cp0::compare            ()
//=============================================================================
{
    return __builtin_mfc0(11, 0);
}

//=============================================================================
    void            Cp0::count              (uint32_t v)
//=============================================================================
{
    __builtin_mtc0(9, 0, v);
}

//=============================================================================
    void            Cp0::compare            (uint32_t v)
//=============================================================================
{
    __builtin_mtc0(11, 0, v);
}

//=============================================================================
    void            Cp0::compare_reload     ()
//=============================================================================
{
    compare(count() + m_compare_count);
}

//=============================================================================
    void            Cp0::compare_irq        (bool tf)
//=============================================================================
{
    Irq::flag_clr(Irq::CORE_TIMER);
    Irq::on(Irq::CORE_TIMER, tf);
}

//=============================================================================
    void            Cp0::compare_us         (uint32_t v)
//=============================================================================
{
    m_compare_count = Osc::sysclk() / 2000000 * v; //cpu speed from Osc::
    compare_reload();
}

//=============================================================================
    void            Cp0::compare_ms         (uint16_t v)
//=============================================================================
{
    compare_us(v * 1000);
}