#include "Spi123.hpp"

/*=============================================================================
 Spi123 functions
=============================================================================*/

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixcon

void Spi123::frame(bool tf){
    r.setbit(m_spixcon, FRMEN, tf);
}
void Spi123::frame_dir(bool tf){
    r.setbit(m_spixcon, FRMSYNC, tf);
}
void Spi123::frame_pol(bool tf){
    r.setbit(m_spixcon, FRMPOL, tf);
}
void Spi123::slave_sel(bool tf){
    r.setbit(m_spixcon, MSSEN, tf);
}
void Spi123::frame_pwidth(bool tf){
    r.setbit(m_spixcon, FRMSYPW, tf);
}
void Spi123::frame_count(FRMCNT e){
    r.clrbit(m_spixcon, FRMCNT_CLR<<FRMCNT_SHIFT);
    r.setbit(m_spixcon, e<<FRMCNT_SHIFT);
}
void Spi123::clk_sel(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon, MCLKSEL, tf);
    freq(); //recaluclate
    on(ison);
}
bool Spi123::clk_sel(){
    return r.anybit(m_spixcon, MCLKSEL);
}
void Spi123::frame_edge(bool tf){
    r.setbit(m_spixcon, SPIFE, tf);
}
void Spi123::enhanced(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon, ENHBUF, tf);
    on(ison);
}
void Spi123::on(bool tf){
    r.setbit(m_spixcon, ON, tf);
}
void Spi123::mode(MODE e){
    r.clrbit(m_spixcon, MODE_CLR<<MODE_SHIFT);
    r.setbit(m_spixcon, e<<MODE_SHIFT);
}
void Spi123::phase(bool tf){
    r.setbit(m_spixcon, SMP, tf);
}
void Spi123::clk_edge(bool tf){
    r.setbit(m_spixcon, CKE, tf);
}
void Spi123::ss(bool tf){
    r.setbit(m_spixcon, SSEN, tf);
}
void Spi123::clk_pol(bool tf){
    r.setbit(m_spixcon, CKP, tf);
}
void Spi123::master(bool tf){
    r.setbit(m_spixcon, MSTEN, tf);
}
void Spi123::tx_irq(TXIRQ e){
    r.clrbit(m_spixcon, STXISEL_CLR<<STXISEL_SHIFT);
    r.setbit(m_spixcon, e<<STXISEL_SHIFT);
}
void Spi123::rx_irq(RXIRQ e){
    r.clrbit(m_spixcon, SRXISEL_CLR<<SRXISEL_SHIFT);
    r.setbit(m_spixcon, e<<SRXISEL_SHIFT);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixstat

uint8_t Spi123::stat_rxcount(){
    return r.val8(m_spixstat+3);
}
uint8_t Spi123::stat_txcount(){
    return r.val8(m_spixstat+2);
}
bool Spi123::stat_frame(){
    return r.anybit(m_spixstat, FRMERR);
}
void Spi123::stat_frame(bool tf){ //always clr
    r.clrbit(m_spixstat, FRMERR);
}
bool Spi123::stat_busy(){
    return r.anybit(m_spixstat, SPIBUSY);
}
bool Spi123::stat_txurun(){
    return r.anybit(m_spixstat, SPITUR);
}
bool Spi123::stat_sremty(){
    return r.anybit(m_spixstat, SRMT);
}
bool Spi123::stat_oflow(){
    return r.anybit(m_spixstat, SPIROV);
}
void Spi123::stat_oflow(bool){ //always clear
    r.clrbit(m_spixstat, SPIROV);
}
bool Spi123::stat_rxemty(){
    return r.anybit(m_spixstat, SPIRBE);
}
bool Spi123::stat_txemty(){
    return r.anybit(m_spixstat, SPITBE);
}
bool Spi123::stat_txfull(){
    return r.anybit(m_spixstat, SPITBF);
}
bool Spi123::stat_rxfull(){
     return r.anybit(m_spixstat, SPIRBF);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixbrg

void Spi123::baud(uint16_t v){
    m_spixbrg = v; //m_spibrg&
}
//set frequency
void Spi123::freq(uint32_t v){
    uint32_t clk;
    if(clk_sel() == REFOCLK) clk = Osc::refo_freq();
    else clk = Osc::sysclk();
    uint16_t brg = (2 * v / clk) - 1;
    brg &= 0x1ff;
    baud(brg);
    freq();
}
//get actual frequency
//called by clk_sel(), freq(uint32_t)
uint32_t Spi123::freq(){
    uint32_t clk;
    if(clk_sel() == REFOCLK) clk = Osc::refo_freq();
    else clk = Osc::sysclk();
    m_spix_freq = clk / (2 * m_spixbrg + 1); //m_spibrg&
    return m_spix_freq;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixcon2

enum AUDMOD : uint8_t { I2S = 0, LEFT, RIGHT, PCMDSP };

void Spi123::sign_ext(bool tf){
    r.setbit(m_spixcon2, SPISGNEXT, tf);
}
void Spi123::irq_frmerr(bool tf){
    r.setbit(m_spixcon2, FRMERREN, tf);
}
void Spi123::irq_oflow(bool tf){
    r.setbit(m_spixcon2, SPIROVEN, tf);
}
void Spi123::irq_urun(bool tf){
    r.setbit(m_spixcon2, SPITUREN, tf);
}
void Spi123::ign_oflow(bool tf){
    r.setbit(m_spixcon2, IGNROV, tf);
}
void Spi123::ign_urun(bool tf){
    r.setbit(m_spixcon2, IGNTUR, tf);
}
void Spi123::audio(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon2, AUDEN, tf);
    on(ison);
}
void Spi123::mono(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon2, AUDOMONO, tf);
    on(ison);
}
void Spi123::audio_mode(AUDMOD e){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.clrbit(m_spixcon2, PCMDSP);
    r.setbit(m_spixcon2, e);
    on(ison);
}