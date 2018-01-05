#include "Timer23.hpp"

/*=============================================================================
 Timer23 functions
=============================================================================*/
void Timer23::timer(uint16_t n) const {
    r.val(m_pt + TMRX, n);
}
uint16_t Timer23::timer() const {
    return r.val16(m_pt + TMRX);
}
void Timer23::pr(uint16_t n) const {
    r.val(m_pt + PRX, n);
}
uint16_t Timer23::pr() const {
    return r.val16(m_pt + PRX);
}
void Timer23::on(bool tf) const {
    r.setbit(m_pt, ON, tf);
}
void Timer23::stop_idle(bool tf) const {
    r.setbit(m_pt, SIDL, tf);
}
void Timer23::tgate(bool tf) const {
    r.setbit(m_pt, TGATE, tf);
}
void Timer23::prescale(PRESCALE e) const {
    r.clrbit(m_pt, PS256);
    r.setbit(m_pt, e);
}
void Timer23::t32bit(bool tf) const {
    r.setbit(m_pt, T32, tf);
}
void Timer23::clk_src(CLK e) const {
    r.setbit(m_pt, TCS, e);
}
