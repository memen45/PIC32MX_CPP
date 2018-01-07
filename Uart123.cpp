#include "Uart123.hpp"

/*=============================================================================
 Uart123 functions
=============================================================================*/

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxmode

void Uart123::stop_sleep(bool tf){
    r.setbit(m_uartxmode, SLPEN, !tf);
}
bool Uart123::active(){
    return r.anybit(m_uartxmode, ACTIVE);
}
void Uart123::clk_sel(CLKSEL e){
    r.clrbit(m_uartxmode, CLKSEL_CLR<<CLKSEL_SHIFT);
    r.setbit(m_uartxmode, e<<CLKSEL_SHIFT);
    update_brg();
}
void Uart123::oflow_stop(bool tf){
    r.setbit(m_uartxmode, OVFDIS, !tf);
}
void Uart123::on(bool tf){
    update_brg();
    r.setbit(m_uartxmode, ON, tf);
}
void Uart123::stop_idle(bool tf){
    r.setbit(m_uartxmode, SIDL, tf);
}
void Uart123::irda(bool tf){
    r.setbit(m_uartxmode, IREN, tf);
}
void Uart123::rts_mode(bool tf){
    r.setbit(m_uartxmode, RTSMD, tf);
}
void Uart123::wake(bool tf){
    r.setbit(m_uartxmode, WAKE, tf);
}
void Uart123::loopback(bool tf){
    r.setbit(m_uartxmode, LPBACK, tf);
}
void Uart123::autobaud(bool tf){
    r.setbit(m_uartxmode, ABAUD, tf);
}
void Uart123::rx_pol(bool tf){
    r.setbit(m_uartxmode, RXINV, tf);
}
void Uart123::brg_mode(bool tf){
    r.setbit(m_uartxmode, BRGH, tf);
    update_brg();
}
void Uart123::mode(MODESEL e){
    r.clrbit(m_uartxmode, MODE_CLR);
    r.setbit(m_uartxmode, e);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxsta



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxbrg

void Uart123::set_baud(uint32_t v){
    m_uartx_baud = v;
    uint8_t bdiv = r.anybit(m_uartxmode, BRGH) ? 4 : 16;
    m_uartxbrg = clk_freq() / v / bdiv - 1;
}
//called by clk_sel(), on(), brg_mode()
void Uart123::update_brg(){
    //if baud not set, set it to 115200
    if(m_uartx_baud == 0) set_baud(115200);
    else set_baud(m_uartx_baud);
}
uint32_t Uart123::clk_freq(){
    CLKSEL e = (CLKSEL)((r.val(m_uartxmode)>>17) & CLKSEL_CLR);
    if(e == REFO1) return Osc::refo_freq();
    else if(e == FRC) return Osc::frcclk();
    return Osc::sysclk();
}
