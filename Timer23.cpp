#include "Timer23.hpp"

enum {
T2CON = 0xBF808040, TMR23_SPACING = 0x10,  //spacing in words
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
            : m_txcon( (vu32ptr)T2CON + (e * TMR23_SPACING) ),
              m_tmrx( *((vu32ptr)T2CON + (e * TMR23_SPACING) + 4) ),
              m_prx( *((vu32ptr)T2CON + (e * TMR23_SPACING) + 8) )
            {
            }

//=============================================================================
            auto Timer23::
count       (uint32_t n) -> void
            {
            m_tmrx = n;
            }

//=============================================================================
            auto Timer23::
count       () -> uint32_t
            {
            return m_tmrx;
            }

//=============================================================================
            auto Timer23::
period      (uint32_t n) -> void
            {
            m_prx = n;
            }

//=============================================================================
            auto Timer23::
period      () -> uint32_t
            {
            return m_prx;
            }

//=============================================================================
            auto Timer23::
on          (bool tf) -> void
            {
            setbit(m_txcon, 1<<ON, tf);
            }

//=============================================================================
            auto Timer23::
stop_idle   (bool tf) -> void
            {
            setbit(m_txcon, 1<<SIDL, tf);
            }

//=============================================================================
            auto Timer23::
gate        (bool tf) -> void
            {
            setbit(m_txcon, 1<<TGATE, tf);
            }

//=============================================================================
            auto Timer23::
prescale    (PRESCALE e) -> void
            {
            clrbit(m_txcon, TCKPS_CLR<<TCKPS_SHIFT);
            setbit(m_txcon, e<<TCKPS_SHIFT);
            }

//=============================================================================
            auto Timer23::
mode32      (bool tf) -> void
            {
            //T2 controls T32, so only do if this is T2
            if(m_txcon == (vu32ptr)T2CON){
                setbit(m_txcon, 1<<T32, tf);
                //set T3 SIDL off
                clrbit(m_txcon + TMR23_SPACING, SIDL);
            }
            }

//=============================================================================
            auto Timer23::
clk_src     (CLK e) -> void
            {
            setbit(m_txcon, 1<<TCS, e);
            }
