#include "Pins.hpp"

/*=============================================================================
 Pins functions
=============================================================================*/
void Pins::lowison(bool tf){
    m_lowison = tf;
}
void Pins::digital_in() const {
    r.setbit(m_pt+TRIS, m_pn);
    r.clrbit(m_pt, m_pn);
}
void Pins::analog_in() const {
    r.setbit(m_pt+TRIS, m_pn);
    r.setbit(m_pt, m_pn);
}
void Pins::digital_out() const {
    r.clrbit(m_pt+TRIS, m_pn);
    r.clrbit(m_pt, m_pn);
}
void Pins::odrain(bool tf) const {
    r.setbit(m_pt+ODC, m_pn, tf);
}
void Pins::pullup(bool tf) const {
    r.setbit(m_pt+CNPU, m_pn, tf);
}
void Pins::pulldn(bool tf) const {
    r.setbit(m_pt+CNPD, m_pn, tf);
}
void Pins::icn(bool tf) const {
    r.setbit(m_pt+CNCON, ON, tf);
}
void Pins::icn_rising() const {
    r.setbit(m_pt+CNCON, CNSTYLE);
    r.setbit(m_pt+CNEN0, m_pn);
    r.clrbit(m_pt+CNEN1, m_pn);
}
void Pins::icn_risefall() const {
    r.setbit(m_pt+CNCON, CNSTYLE);
    r.setbit(m_pt+CNEN0, m_pn);
    r.clrbit(m_pt+CNEN1, m_pn);
}
void Pins::icn_falling() const {
    r.setbit(m_pt+CNCON, CNSTYLE);
    r.setbit(m_pt+CNEN1, m_pn);
    r.clrbit(m_pt+CNEN0, m_pn);
}
void Pins::icn_mismatch() const {
    r.setbit(m_pt+CNEN0, m_pn);
    r.setbit(m_pt+CNCON, CNSTYLE, 0);
}
bool Pins::icn_flag() const {
    return r.anybit(m_pt+CNF, m_pn);
}
bool Pins::icn_stat() const {
    return r.anybit(m_pt+CNSTAT, m_pn);
}

/*=============================================================================
 Pins functions - static
=============================================================================*/
//unlock, write byte, lock
void Pins::pps_do(uint32_t addr, uint8_t v){
    sk.unlock();
    r.clrbit(RPCON, IOLOCK);
    r.val(addr, v);
    r.setbit(RPCON, IOLOCK);
    sk.lock();
}
//pin -> pps peripheral in
void Pins::pps_in(PPSIN e, RPN n){
    pps_do(RPINR1+((e/4)*16)+(e%4), n&31);
    r.setbit(ANSELA + TRIS + ((n>>8)/16)*0x100, 1<<((n>>8)%16));  //tris=1
    r.clrbit(ANSELA + ((n>>8)/16)*0x100, 1<<((n>>8)%16));      //ansel=0
}
//pps peripheral out -> pin
void Pins::pps_out(PPSOUT e, RPN n){
    pps_do(RPOR0+((((n&31)-1)/4)*16)+(((n&31)-1)%4), e);
}

