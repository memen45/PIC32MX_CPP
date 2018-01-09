#include "Uart123.hpp"

/*=============================================================================
 Uart123 functions
=============================================================================*/

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxmode

void Uart123::stop_sleep(bool tf){
    r.setbit(m_uartx_mode, SLPEN, !tf);
}
bool Uart123::active(){
    return r.anybit(m_uartx_mode, ACTIVE);
}
void Uart123::clk_sel(CLKSEL e){
    r.clrbit(m_uartx_mode, CLKSEL_CLR<<CLKSEL_SHIFT);
    r.setbit(m_uartx_mode, e<<CLKSEL_SHIFT);
    update_brg();
}
void Uart123::oflow_stop(bool tf){
    r.setbit(m_uartx_mode, OVFDIS, !tf);
}
void Uart123::on(bool tf){
    update_brg(); //in case not set
    r.setbit(m_uartx_mode, ON, tf);
}
void Uart123::stop_idle(bool tf){
    r.setbit(m_uartx_mode, SIDL, tf);
}
void Uart123::irda(bool tf){
    r.setbit(m_uartx_mode, IREN, tf);
}
void Uart123::rts_mode(RTSMODE e){
    r.setbit(m_uartx_mode, RTSMD, e);
}
void Uart123::wake(bool tf){
    r.setbit(m_uartx_mode, WAKE, tf);
}
void Uart123::loopback(bool tf){
    r.setbit(m_uartx_mode, LPBACK, tf);
}
void Uart123::autobaud(bool tf){
    r.setbit(m_uartx_mode, ABAUD, tf);
}
void Uart123::rx_pol(RXPOL e){
    r.setbit(m_uartx_mode, RXINV, e);
}
void Uart123::speed(SPEED e){
    r.setbit(m_uartx_mode, BRGH, e);
    update_brg();
}
void Uart123::mode(MODESEL e){
    r.clrbit(m_uartx_mode, MODE_CLR);
    r.setbit(m_uartx_mode, e);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxsta



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxbrg

void Uart123::set_baud(uint32_t v){
    m_uartx_baud = v;
    uint8_t bdiv = r.anybit(m_uartx_mode, BRGH) ? 4 : 16;
    m_uartx_brg = clk_freq() / v / bdiv - 1;
}
//called by clk_sel(), on(), brg_mode()
void Uart123::update_brg(){
    //if baud not set, set it to 115200
    if(m_uartx_baud == 0) set_baud(115200);
    else set_baud(m_uartx_baud);
}
uint32_t Uart123::clk_freq(){
    CLKSEL e = (CLKSEL)((r.val(m_uartx_mode)>>17) & CLKSEL_CLR);
    if(e == REFO1) return Osc::refo_freq();
    else if(e == FRC) return Osc::frcclk();
    return Osc::sysclk(); //pb/sys are the same
}
