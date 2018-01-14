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
    r.val((vbyte_ptr)m_uartx_stat+3, v);
}
void Uart::rx_addr(uint8_t v){
    r.val((vbyte_ptr)m_uartx_stat+2, v);
}
void Uart::tx_irq(UTXISEL e){
    r.clrbit(m_uartx_stat, UTXISEL_CLR<<UTXISEL_SHIFT);
    r.setbit(m_uartx_stat, e<<UTXISEL_SHIFT);
}
void Uart::tx_pol(RXPOL e){
    bool b = r.anybit(m_uartx_mode, IREN) ? !e : e;
    r.setbit(m_uartx_stat, UTXINV, b);
}
void Uart::rx_on(bool tf){
    r.setbit(m_uartx_stat, URXEN, tf);
}
void Uart::tx_break(){
    r.setbit(m_uartx_stat, UTXBRK);
}
void Uart::tx_on(bool tf){
    r.setbit(m_uartx_stat, UTXEN, tf);
}
bool Uart::tx_full(){
    return r.anybit(m_uartx_stat, UTXBF);
}
bool Uart::tx_done(){
    return r.anybit(m_uartx_stat, TRMT);
}
void Uart::rx_irq(URXISEL e){
    r.clrbit(m_uartx_stat, URXISEL_CLR<<URXISEL_SHIFT);
    r.setbit(m_uartx_stat, e<<URXISEL_SHIFT);
}
void Uart::rx_addren(bool tf){
    r.setbit(m_uartx_stat, ADDEN, tf);
}
bool Uart::rx_busy(){
    return !r.anybit(m_uartx_stat, RIDLE);
}
bool Uart::rx_perr(){
    return r.anybit(m_uartx_stat, PERR);
}
bool Uart::rx_ferr(){
    return r.anybit(m_uartx_stat, FERR);
}
bool Uart::rx_oerr(){
    return r.anybit(m_uartx_stat, OERR);
}
bool Uart::rx_empty(){
    return !r.anybit(m_uartx_stat, URXDA);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxbrg

void Uart::baud_set(uint32_t v){
    m_uartx_baud = v;
    uint8_t bdiv = r.anybit(m_uartx_mode, BRGH) ? 4 : 16;
    m_uartx_brg = baud_clk() / v / bdiv - 1;
}
//called by clk_sel(), on(), brg_mode()
void Uart::baud_set(){
    //if baud not set, set it to 115200
    if(m_uartx_baud == 0) baud_set(115200);
    else baud_set(m_uartx_baud);
}
uint32_t Uart::baud_clk(){
    CLKSEL e = (CLKSEL)((r.val(m_uartx_mode)>>17) & CLKSEL_CLR);
    if(e == REFO1) return Osc::refo_freq();
    else if(e == FRC) return Osc::frcclk();
    return Osc::sysclk(); //pb/sys are the same
}
