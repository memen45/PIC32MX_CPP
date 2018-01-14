#include "Uart.hpp"

/*=============================================================================
 Uart functions
=============================================================================*/

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxmode

void Uart::stop_sleep(bool tf){
    r.setbit(m_uartx_mode, SLPEN, !tf);
}
bool Uart::active(){
    return r.anybit(m_uartx_mode, ACTIVE);
}
void Uart::clk_sel(CLKSEL e){
    r.clrbit(m_uartx_mode, CLKSEL_CLR<<CLKSEL_SHIFT);
    r.setbit(m_uartx_mode, e<<CLKSEL_SHIFT);
    baud_set();
}
void Uart::oflow_stop(bool tf){
    r.setbit(m_uartx_mode, OVFDIS, !tf);
}
void Uart::on(bool tf){
    baud_set(); //in case not set
    r.setbit(m_uartx_mode, ON, tf);
}
void Uart::stop_idle(bool tf){
    r.setbit(m_uartx_mode, SIDL, tf);
}
void Uart::irda(bool tf){
    r.setbit(m_uartx_mode, IREN, tf);
}
void Uart::rts_mode(RTSMODE e){
    r.setbit(m_uartx_mode, RTSMD, e);
}
void Uart::wake(bool tf){
    r.setbit(m_uartx_mode, WAKE, tf);
}
void Uart::loopback(bool tf){
    r.setbit(m_uartx_mode, LPBACK, tf);
}
void Uart::autobaud(bool tf){
    r.setbit(m_uartx_mode, ABAUD, tf);
}
void Uart::rx_pol(RXPOL e){
    r.setbit(m_uartx_mode, RXINV, e);
}
void Uart::hispeed(bool tf){
    r.setbit(m_uartx_mode, BRGH, tf);
    baud_set();
}
void Uart::mode(MODESEL e){
    r.clrbit(m_uartx_mode, MODE_CLR<<MODE_SHIFT);
    r.setbit(m_uartx_mode, e<<MODE_SHIFT);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxsta

void Uart::rx_mask(uint8_t v){
    r.val((vbyte_ptr)m_uartx_mode+(UXSTA*4)+3, v);
}
void Uart::rx_addr(uint8_t v){
    r.val((vbyte_ptr)m_uartx_mode+(UXSTA*4)+2, v);
}
void Uart::tx_irq(UTXISEL e){
    r.clrbit(m_uartx_mode+UXSTA, UTXISEL_CLR<<UTXISEL_SHIFT);
    r.setbit(m_uartx_mode+UXSTA, e<<UTXISEL_SHIFT);
}
void Uart::tx_pol(RXPOL e){
    bool b = r.anybit(m_uartx_mode, IREN) ? !e : e;
    r.setbit(m_uartx_mode+UXSTA, UTXINV, b);
}
void Uart::rx_on(bool tf){
    r.setbit(m_uartx_mode+UXSTA, URXEN, tf);
}
void Uart::tx_break(){
    r.setbit(m_uartx_mode+UXSTA, UTXBRK);
}
void Uart::tx_on(bool tf){
    r.setbit(m_uartx_mode+UXSTA, UTXEN, tf);
}
bool Uart::tx_full(){
    return r.anybit(m_uartx_mode+UXSTA, UTXBF);
}
bool Uart::tx_done(){
    return r.anybit(m_uartx_mode+UXSTA, TRMT);
}
void Uart::rx_irq(URXISEL e){
    r.clrbit(m_uartx_mode+UXSTA, URXISEL_CLR<<URXISEL_SHIFT);
    r.setbit(m_uartx_mode+UXSTA, e<<URXISEL_SHIFT);
}
void Uart::rx_addren(bool tf){
    r.setbit(m_uartx_mode+UXSTA, ADDEN, tf);
}
bool Uart::rx_busy(){
    return !r.anybit(m_uartx_mode+UXSTA, RIDLE);
}
bool Uart::rx_perr(){
    return r.anybit(m_uartx_mode+UXSTA, PERR);
}
bool Uart::rx_ferr(){
    return r.anybit(m_uartx_mode+UXSTA, FERR);
}
bool Uart::rx_oerr(){
    return r.anybit(m_uartx_mode+UXSTA, OERR);
}
bool Uart::rx_empty(){
    return !r.anybit(m_uartx_mode+UXSTA, URXDA);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxbrg

void Uart::baud_set(uint32_t v){
    m_uartx_baud = v;
    uint8_t bdiv = r.anybit(m_uartx_mode, BRGH) ? 4 : 16;
    v = baud_clk() / v / bdiv - 1;
    r.val(m_uartx_mode+UXBRG, v);
}
//called by clk_sel(), on(), brg_mode()
void Uart::baud_set(){
    //if baud not set, set it to 115200
    baud_set(m_uartx_baud ? m_uartx_baud : 115200);
}
uint32_t Uart::baud_clk(){
    CLKSEL e = (CLKSEL)((r.val(m_uartx_mode)>>17) & CLKSEL_CLR);
    if(e == REFO1) return Osc::refo_freq();
    else if(e == FRC) return Osc::frcclk();
    return Osc::sysclk(); //pb/sys are the same
}
