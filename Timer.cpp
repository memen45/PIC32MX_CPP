#include "Timer.hpp"
#include "Reg.hpp"

enum {
    T2CON = 0xBF800800, TMRX_SPACING = 0x80,  //spacing in words
    ON = 15,
    SIDL = 13,
    TGATE = 7,
    TCKPS_SHIFT = 4, TCKPS_MASK = 7,
    T32 = 3,
    TCS = 1
};

//Timer
//=============================================================================
            Timer::
Timer       (TMRX e)
            : 
            m_txcon( (vu32ptr)T2CON + (e * TMRX_SPACING) ),
            m_tmrx( *((vu32ptr)T2CON + (e * TMRX_SPACING) + 4) ),
            m_prx( *((vu32ptr)T2CON + (e * TMRX_SPACING) + 8) )
            {
            }

//=============================================================================
            auto Timer::
count       (uint32_t n) -> void
            {
            m_tmrx = n;
            if(Reg::anybit(m_txcon, 1<<T32)){
                *((vu32ptr)m_tmrx + TMRX_SPACING) = n>>16;
            }
            }

//=============================================================================
            auto Timer::
count       () -> uint32_t
            {
            uint32_t ret = 0;
            if(Reg::anybit(m_txcon, 1<<T32)){
                ret = *((vu32ptr)m_tmrx + TMRX_SPACING)<<16;
            }
            return ret | m_tmrx;
            }

//=============================================================================
            auto Timer::
period      (uint32_t n) -> void
            {
            m_prx = n;
            if(Reg::anybit(m_txcon, 1<<T32)){
                *((vu32ptr)m_prx + TMRX_SPACING) = n>>16;
            }
            }

//=============================================================================
            auto Timer::
period      () -> uint32_t
            {
            uint32_t ret = 0;
            if(Reg::anybit(m_txcon, 1<<T32)){
                ret = *((vu32ptr)m_prx + TMRX_SPACING)<<16;
            }
            return ret | m_prx;
            }

//=============================================================================
            auto Timer::
on          (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<ON, tf);
            }

//=============================================================================
            auto Timer::
stop_idle   (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<SIDL, tf);
            }

//=============================================================================
            auto Timer::
gate        (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<TGATE, tf);
            }

//=============================================================================
            auto Timer::
prescale    (PRESCALE e) -> void
            {
            Reg::clrbit(m_txcon, TCKPS_MASK<<TCKPS_SHIFT);
            Reg::setbit(m_txcon, e<<TCKPS_SHIFT);
            }
            
//=============================================================================
            auto Timer::
prescale    () -> PRESCALE
            {
            return (PRESCALE) ((Reg::val(m_txcon) >> TCKPS_SHIFT) bitand TCKPS_MASK);
            }

//=============================================================================
            auto Timer::
mode32      (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<T32, tf);
            //if turned on, bit will 'stick'
            //so must be T2, make sure T3 SIDL is off
            if(tf and Reg::anybit(m_txcon, 1<<T32)){
            Reg::clrbit(m_txcon + TMRX_SPACING, SIDL);
            }
            }

//=============================================================================
            auto Timer::
clk_src     (CLK e) -> void
            {
            Reg::setbit(m_txcon, 1<<TCS, e);
            }
