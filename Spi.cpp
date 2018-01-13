#include "Spi.hpp"

/*=============================================================================
 Spi functions
=============================================================================*/

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixcon

void Spi::frame(bool tf){
    r.setbit(m_spixcon, FRMEN, tf);
}
void Spi::frame_dir(FRMDIR e){
    r.setbit(m_spixcon, FRMSYNC, e);
}
void Spi::frame_pol(FRMHL e){
    r.setbit(m_spixcon, FRMPOL, e);
}
void Spi::slave_sel(bool tf){
    r.setbit(m_spixcon, MSSEN, tf);
}
void Spi::frame_pwidth(FRMPW e){
    r.setbit(m_spixcon, FRMSYPW, e);
}
void Spi::frame_count(FRMCNT e){
    r.clrbit(m_spixcon, FRMCNT_CLR<<FRMCNT_SHIFT);
    r.setbit(m_spixcon, e<<FRMCNT_SHIFT);
}
void Spi::clk_sel(CLKSEL e){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon, MCLKSEL, e);
    freq(); //recaluclate
    on(ison);
}
auto Spi::clk_sel() -> CLKSEL {
    return (CLKSEL)r.anybit(m_spixcon, MCLKSEL);
}
void Spi::frame_edge(FRMEDGE e){
    r.setbit(m_spixcon, SPIFE, e);
}
void Spi::enhanced(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon, ENHBUF, tf);
    on(ison);
}
void Spi::on(bool tf){
    r.setbit(m_spixcon, ON, tf);
}
void Spi::mode(MODE e){
    r.clrbit(m_spixcon, MODE_CLR<<MODE_SHIFT);
    r.setbit(m_spixcon, e<<MODE_SHIFT);
}
void Spi::phase(PHASE e){
    r.setbit(m_spixcon, SMP, e);
}
void Spi::clk_edge(CLKEDGE e){
    r.setbit(m_spixcon, CKE, e);
}
void Spi::ss(bool tf){
    r.setbit(m_spixcon, SSEN, tf);
}
void Spi::clk_pol(CLKPOL e){
    r.setbit(m_spixcon, CKP, e);
}
void Spi::master(bool tf){
    r.setbit(m_spixcon, MSTEN, tf);
}
void Spi::tx_irq(TXIRQ e){
    r.clrbit(m_spixcon, STXISEL_CLR<<STXISEL_SHIFT);
    r.setbit(m_spixcon, e<<STXISEL_SHIFT);
}
void Spi::rx_irq(RXIRQ e){
    r.clrbit(m_spixcon, SRXISEL_CLR<<SRXISEL_SHIFT);
    r.setbit(m_spixcon, e<<SRXISEL_SHIFT);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixstat

uint8_t Spi::stat_rxcount(){
    return r.val8(m_spixstat+3);
}
uint8_t Spi::stat_txcount(){
    return r.val8(m_spixstat+2);
}
bool Spi::stat_frame(){
    return r.anybit(m_spixstat, FRMERR);
}
void Spi::stat_frame(bool tf){ //always clr
    r.clrbit(m_spixstat, FRMERR);
}
bool Spi::stat_busy(){
    return r.anybit(m_spixstat, SPIBUSY);
}
bool Spi::stat_txurun(){
    return r.anybit(m_spixstat, SPITUR);
}
bool Spi::stat_sremty(){
    return r.anybit(m_spixstat, SRMT);
}
bool Spi::stat_oflow(){
    return r.anybit(m_spixstat, SPIROV);
}
void Spi::stat_oflow(bool){ //always clear
    r.clrbit(m_spixstat, SPIROV);
}
bool Spi::stat_rxemty(){
    return r.anybit(m_spixstat, SPIRBE);
}
bool Spi::stat_txemty(){
    return r.anybit(m_spixstat, SPITBE);
}
bool Spi::stat_txfull(){
    return r.anybit(m_spixstat, SPITBF);
}
bool Spi::stat_rxfull(){
     return r.anybit(m_spixstat, SPIRBF);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixbrg

void Spi::baud(uint16_t v){
    m_spixbrg = v; //m_spibrg&
}
//set frequency
void Spi::freq(uint32_t v){
    uint32_t clk;
    if(clk_sel() == REFO1) clk = Osc::refo_freq();
    else clk = Osc::sysclk();
    uint16_t brg = (2 * v / clk) - 1;
    brg &= 0x1ff;
    baud(brg);
    freq();
}
//get actual frequency
//called by clk_sel(), freq(uint32_t)
uint32_t Spi::freq(){
    uint32_t clk;
    if(clk_sel() == REFO1) clk = Osc::refo_freq();
    else clk = Osc::sysclk();
    m_spix_freq = clk / (2 * m_spixbrg + 1); //m_spibrg&
    return m_spix_freq;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixcon2

enum AUDMOD : uint8_t { I2S = 0, LEFT, RIGHT, PCMDSP };

void Spi::sign_ext(bool tf){
    r.setbit(m_spixcon2, SPISGNEXT, tf);
}
void Spi::irq_frmerr(bool tf){
    r.setbit(m_spixcon2, FRMERREN, tf);
}
void Spi::irq_oflow(bool tf){
    r.setbit(m_spixcon2, SPIROVEN, tf);
}
void Spi::irq_urun(bool tf){
    r.setbit(m_spixcon2, SPITUREN, tf);
}
void Spi::ign_oflow(bool tf){
    r.setbit(m_spixcon2, IGNROV, tf);
}
void Spi::ign_urun(bool tf){
    r.setbit(m_spixcon2, IGNTUR, tf);
}
void Spi::audio(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon2, AUDEN, tf);
    on(ison);
}
void Spi::mono(bool tf){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.setbit(m_spixcon2, AUDOMONO, tf);
    on(ison);
}
void Spi::audio_mode(AUDMOD e){
    bool ison = r.anybit(m_spixcon, ON);
    on(false);
    r.clrbit(m_spixcon2, PCMDSP);
    r.setbit(m_spixcon2, e);
    on(ison);
}