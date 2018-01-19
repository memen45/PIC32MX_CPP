#include "Spi.hpp"

//Spi

//spixcon
//.............................................................................
void Spi::frame(bool tf)
//.............................................................................
{
    r.setbit(m_spix_con, FRMEN, tf);
}
//.............................................................................
void Spi::frame_dir(FRMDIR e)
//.............................................................................
{
    r.setbit(m_spix_con, FRMSYNC, e);
}
//.............................................................................
void Spi::frame_pol(FRMHL e)
//.............................................................................
{
    r.setbit(m_spix_con, FRMPOL, e);
}
//.............................................................................
void Spi::slave_sel(bool tf)
//.............................................................................
{
    r.setbit(m_spix_con, MSSEN, tf);
}
//.............................................................................
void Spi::frame_pwidth(FRMPW e)
//.............................................................................
{
    r.setbit(m_spix_con, FRMSYPW, e);
}
//.............................................................................
void Spi::frame_count(FRMCNT e)
//.............................................................................
{
    r.clrbit(m_spix_con, FRMCNT_CLR<<FRMCNT_SHIFT);
    r.setbit(m_spix_con, e<<FRMCNT_SHIFT);
}
//.............................................................................
void Spi::clk_sel(CLKSEL e)
//.............................................................................
{
    bool ison = r.anybit(m_spix_con, ON);
    on(false);
    r.setbit(m_spix_con, MCLKSEL, e);
    freq(); //recaluclate
    on(ison);
}
//.............................................................................
auto Spi::clk_sel() -> CLKSEL
//.............................................................................
{
    return (CLKSEL)r.anybit(m_spix_con, MCLKSEL);
}
//.............................................................................
void Spi::frame_edge(FRMEDGE e)
//.............................................................................
{
    r.setbit(m_spix_con, SPIFE, e);
}
//.............................................................................
void Spi::enhanced(bool tf)
//.............................................................................
{
    bool ison = r.anybit(m_spix_con, ON);
    on(false);
    r.setbit(m_spix_con, ENHBUF, tf);
    on(ison);
}
//.............................................................................
void Spi::on(bool tf)
//.............................................................................
{
    r.setbit(m_spix_con, ON, tf);
}
//.............................................................................
void Spi::mode(MODE e)
//.............................................................................
{
    r.clrbit(m_spix_con, MODE_CLR<<MODE_SHIFT);
    r.setbit(m_spix_con, e<<MODE_SHIFT);
}
//.............................................................................
void Spi::phase(PHASE e)
//.............................................................................
{
    r.setbit(m_spix_con, SMP, e);
}
//.............................................................................
void Spi::clk_edge(CLKEDGE e)
//.............................................................................
{
    r.setbit(m_spix_con, CKE, e);
}
//.............................................................................
void Spi::ss(bool tf)
//.............................................................................
{
    r.setbit(m_spix_con, SSEN, tf);
}
//.............................................................................
void Spi::clk_pol(CLKPOL e)
//.............................................................................
{
    r.setbit(m_spix_con, CKP, e);
}
//.............................................................................
void Spi::master(bool tf)
//.............................................................................
{
    r.setbit(m_spix_con, MSTEN, tf);
}
//.............................................................................
void Spi::tx_irq(TXIRQ e)
//.............................................................................
{
    r.clrbit(m_spix_con, STXISEL_CLR<<STXISEL_SHIFT);
    r.setbit(m_spix_con, e<<STXISEL_SHIFT);
}
//.............................................................................
void Spi::rx_irq(RXIRQ e)
//.............................................................................
{
    r.clrbit(m_spix_con, SRXISEL_CLR<<SRXISEL_SHIFT);
    r.setbit(m_spix_con, e<<SRXISEL_SHIFT);
}
//spixstat
//.............................................................................
uint8_t Spi::stat_rxcount()
//.............................................................................
{
    return r.val8((vbyte_ptr)m_spix_con+(SPIXSTAT*4)+3);
}
//.............................................................................
uint8_t Spi::stat_txcount()
//.............................................................................
{
    return r.val8((vbyte_ptr)m_spix_con+(SPIXSTAT*4)+2);
}
bool Spi::stat_frame(){
    return r.anybit(m_spix_con+SPIXSTAT, FRMERR);
}
void Spi::stat_frame(bool tf){ //always clr
    r.clrbit(m_spix_con+SPIXSTAT, FRMERR);
}
bool Spi::stat_busy(){
    return r.anybit(m_spix_con+SPIXSTAT, SPIBUSY);
}
bool Spi::stat_txurun(){
    return r.anybit(m_spix_con+SPIXSTAT, SPITUR);
}
bool Spi::stat_sremty(){
    return r.anybit(m_spix_con+SPIXSTAT, SRMT);
}
bool Spi::stat_oflow(){
    return r.anybit(m_spix_con+SPIXSTAT, SPIROV);
}
void Spi::stat_oflow(bool){ //always clear
    r.clrbit(m_spix_con+SPIXSTAT, SPIROV);
}
bool Spi::stat_rxemty(){
    return r.anybit(m_spix_con+SPIXSTAT, SPIRBE);
}
bool Spi::stat_txemty(){
    return r.anybit(m_spix_con+SPIXSTAT, SPITBE);
}
bool Spi::stat_txfull(){
    return r.anybit(m_spix_con+SPIXSTAT, SPITBF);
}
bool Spi::stat_rxfull(){
     return r.anybit(m_spix_con+SPIXSTAT, SPIRBF);
}
//spixbrg
void Spi::baud(uint16_t v){
    r.val(m_spix_con+SPIXBRG, v);
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
    m_spix_freq = clk / (2 * r.val16(m_spix_con+SPIXBRG) + 1);
    return m_spix_freq;
}
//spixcon2
void Spi::sign_ext(bool tf){
    r.setbit(m_spix_con+SPIXCON2, SPISGNEXT, tf);
}
void Spi::irq_frmerr(bool tf){
    r.setbit(m_spix_con+SPIXCON2, FRMERREN, tf);
}
void Spi::irq_oflow(bool tf){
    r.setbit(m_spix_con+SPIXCON2, SPIROVEN, tf);
}
void Spi::irq_urun(bool tf){
    r.setbit(m_spix_con+SPIXCON2, SPITUREN, tf);
}
void Spi::ign_oflow(bool tf){
    r.setbit(m_spix_con+SPIXCON2, IGNROV, tf);
}
void Spi::ign_urun(bool tf){
    r.setbit(m_spix_con+SPIXCON2, IGNTUR, tf);
}
void Spi::audio(bool tf){
    bool ison = r.anybit(m_spix_con, ON);
    on(false);
    r.setbit(m_spix_con+SPIXCON2, AUDEN, tf);
    on(ison);
}
void Spi::mono(bool tf){
    bool ison = r.anybit(m_spix_con, ON);
    on(false);
    r.setbit(m_spix_con+SPIXCON2, AUDOMONO, tf);
    on(ison);
}
void Spi::audio_mode(AUDMOD e){
    bool ison = r.anybit(m_spix_con, ON);
    on(false);
    r.clrbit(m_spix_con+SPIXCON2, PCMDSP);
    r.setbit(m_spix_con+SPIXCON2, e);
    on(ison);
}