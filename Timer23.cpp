#include "Timer23.hpp"
#include "Reg.hpp"

enum {
T1CON = 0xBF808040, TMR23_SPACING = 0x10,  //spacing in words
ON = 15,
SIDL = 13,
TGATE = 7,
TCKPS_SHIFT = 4, TCKPS_CLR = 7,
T32 = 3,
TCS = 1
};


//Timer23
//=============================================================================
            Timer23::
Timer23     (TMR23 e)
            : m_txcon( (vu32ptr)T1CON + (e * TMR23_SPACING) ),
            m_tmrx( *((vu32ptr)T1CON + (e * TMR23_SPACING) + 4) ),
            m_prx( *((vu32ptr)T1CON + (e * TMR23_SPACING) + 8) )
            {
            }

//=============================================================================
            auto Timer23::
count       (uint32_t n) -> void
            {
            m_tmrx = n;
            if(Reg::anybit(m_txcon, 1<<T32)){
                *((vu32ptr)m_tmrx + TMR23_SPACING) = n>>16;
            }
            }

//=============================================================================
            auto Timer23::
count       () -> uint32_t
            {
            uint32_t ret = 0;
            if(Reg::anybit(m_txcon, 1<<T32)){
                ret = *((vu32ptr)m_tmrx + TMR23_SPACING)<<16;
            }
            return ret | m_tmrx;
            }

//=============================================================================
            auto Timer23::
period      (uint32_t n) -> void
            {
            m_prx = n;
            if(Reg::anybit(m_txcon, 1<<T32)){
                *((vu32ptr)m_prx + TMR23_SPACING) = n>>16;
            }
            }

//=============================================================================
            auto Timer23::
period      () -> uint32_t
            {
            uint32_t ret = 0;
            if(Reg::anybit(m_txcon, 1<<T32)){
                ret = *((vu32ptr)m_prx + TMR23_SPACING)<<16;
            }
            return ret | m_prx;
            }

//=============================================================================
            auto Timer23::
on          (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<ON, tf);
            }

//=============================================================================
            auto Timer23::
stop_idle   (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<SIDL, tf);
            }

//=============================================================================
            auto Timer23::
gate        (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<TGATE, tf);
            }

//=============================================================================
            auto Timer23::
prescale    (PRESCALE e) -> void
            {
            Reg::clrbit(m_txcon, TCKPS_CLR<<TCKPS_SHIFT);
            Reg::setbit(m_txcon, e<<TCKPS_SHIFT);
            }

//=============================================================================
            auto Timer23::
mode32      (bool tf) -> void
            {
            Reg::setbit(m_txcon, 1<<T32, tf);
            //if turned on, bit will 'stick'
            //so must be T2, make sure T3 SIDL is off
            if(tf and Reg::anybit(m_txcon, 1<<T32)){
            Reg::clrbit(m_txcon + TMR23_SPACING, SIDL);
            }
            }

//=============================================================================
            auto Timer23::
clk_src     (CLK e) -> void
            {
            Reg::setbit(m_txcon, 1<<TCS, e);
            }
