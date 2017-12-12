#include "Timer23.hpp"

/*=============================================================================
 Timer23 functions
=============================================================================*/

void Timer23::timer(uint16_t n){ Reg::val(m_pt + TMRX, n); }
uint16_t Timer23::timer(){ return Reg::val(m_pt + TMRX); }
void Timer23::pr(uint16_t n){ Reg::val(m_pt + PRX, n); }
uint16_t Timer23::pr(){ return Reg::val(m_pt + PRX); }
void Timer23::on(bool tf){ Reg::set(m_pt, ON, tf); }
void Timer23::stop_idle(bool tf){ Reg::set(m_pt, SIDL, tf); }
void Timer23::tgate(bool tf){ Reg::set(m_pt, TGATE, tf); }
void Timer23::prescale(PRESCALE e){ Reg::clr(m_pt, PS256); Reg::set(m_pt, e); }
void Timer23::t32bit(bool tf){ Reg::set(m_pt, T32, tf); }
void Timer23::clk_src(CLK e){ Reg::set(m_pt, TCS, e); }
