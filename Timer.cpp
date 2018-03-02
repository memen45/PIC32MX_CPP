#include "Timer.hpp"
#include "Reg.hpp"

enum {
    ON = 15,
    SIDL = 13,
    TGATE = 7,
    TCKPS_SHIFT = 4, TCKPS_MASK = 7,
    T32 = 3,
    TCS = 1
};

enum { T2CON = 0xBF800800, TMRX_SPACING = 0x80 }; //spacing in words

using vu32ptr = volatile uint32_t*;


//Timer23
//=============================================================================
                Timer::Timer        (TMRX e)
//=============================================================================
    : m_txcon( (vu32ptr)T2CON + (e * TMRX_SPACING) ),
      m_tmrx( *((vu32ptr)T2CON + (e * TMRX_SPACING) + 4) ),
      m_prx( *((vu32ptr)T2CON + (e * TMRX_SPACING) + 8) )
{
}

//=============================================================================
    void        Timer::count          (uint32_t n) const
//=============================================================================
{
    m_tmrx = n;
    if(Reg::anybit(m_txcon, 1<<T32)){
        *((vu32ptr)m_tmrx + TMRX_SPACING) = n>>16;
    }
}

//=============================================================================
    uint32_t    Timer::count          () const
//=============================================================================
{
    uint32_t ret = 0;
    if(Reg::anybit(m_txcon, 1<<T32)){
        ret = *((vu32ptr)m_tmrx + TMRX_SPACING)<<16;
    }
    return ret | m_tmrx;
}

//=============================================================================
    void        Timer::period         (uint32_t n) const
//=============================================================================
{
    m_prx = n;
    if(Reg::anybit(m_txcon, 1<<T32)){
        *((vu32ptr)m_prx + TMRX_SPACING) = n>>16;
    }
}

//=============================================================================
    uint32_t    Timer::period         () const
//=============================================================================
{
    uint32_t ret = 0;
    if(Reg::anybit(m_txcon, 1<<T32)){
        ret = *((vu32ptr)m_prx + TMRX_SPACING)<<16;
    }
    return ret | m_prx;
}

//=============================================================================
    void        Timer::on             (bool tf) const
//=============================================================================
{
    Reg::setbit(m_txcon, 1<<ON, tf);
}

//=============================================================================
    void        Timer::stop_idle      (bool tf) const
//=============================================================================
{
    Reg::setbit(m_txcon, 1<<SIDL, tf);
}

//=============================================================================
    void        Timer::gate           (bool tf) const
//=============================================================================
{
    Reg::setbit(m_txcon, 1<<TGATE, tf);
}

//=============================================================================
    void        Timer::prescale       (PRESCALE e) const
//=============================================================================
{
    Reg::clrbit(m_txcon, TCKPS_MASK<<TCKPS_SHIFT);
    Reg::setbit(m_txcon, e<<TCKPS_SHIFT);
}
    
//=============================================================================
    auto        Timer::prescale       () -> PRESCALE
//=============================================================================
{
        return (PRESCALE) ((Reg::val(m_txcon) >> TCKPS_SHIFT) bitand TCKPS_MASK);
}    

//=============================================================================
    void        Timer::mode32         (bool tf) const
//=============================================================================
{
    Reg::setbit(m_txcon, 1<<T32, tf);
    //if turned on, bit will 'stick'
    //so must be T2, make sure T3 SIDL is off
    if(tf and Reg::anybit(m_txcon, 1<<T32)){
       Reg::clrbit(m_txcon + TMRX_SPACING, SIDL);
    }
}

//=============================================================================
    void        Timer::clk_src        (CLK e) const
//=============================================================================
{
    Reg::setbit(m_txcon, 1 << TCS, e);
}
