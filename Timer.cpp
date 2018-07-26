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
            }

//=============================================================================
            auto Timer::
count       () -> uint32_t
            {
            return m_tmrx;
            }

//=============================================================================
            auto Timer::
period      (uint32_t n) -> void
            {
            m_prx = n;
            }

//=============================================================================
            auto Timer::
period      () -> uint32_t
            {
            return m_prx;
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
            //T2 controls T32, so only do if this is T2 or T4
            if (m_txcon == (vu32ptr)T2CON || m_txcon == (vu32ptr)T2CON + (2 * TMRX_SPACING)){
                Reg::setbit(m_txcon, 1<<T32, tf);
                //set T3 or T5 SIDL off
                Reg::clrbit(m_txcon + TMRX_SPACING, SIDL);
            }
            }

//=============================================================================
            auto Timer::
clk_src     (CLK e) -> void
            {
            Reg::setbit(m_txcon, 1<<TCS, e);
            }
