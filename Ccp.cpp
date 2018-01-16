#include "Ccp.hpp"

/*=============================================================================
 MCCP/SCCP functions
=============================================================================*/
//ccpxcon1
void Ccp::outscale_src(OPOSTSRC e){
    r.setbit(m_ccpx_con, OPSSRC, e);
}
void Ccp::retrigger(bool tf){
    r.setbit(m_ccpx_con, RTRGEN, tf);
}
void Ccp::outscale(uint8_t v){
    r.clrbit(m_ccpx_con, OPS_CLR<<OPS_SHIFT);
    r.setbit(m_ccpx_con, (v&OPS_CLR)<<OPS_SHIFT);
}
void Ccp::trig_mode(){
    r.setbit(m_ccpx_con, TRIGEN);
}
void Ccp::sync_mode(){
    r.clrbit(m_ccpx_con, TRIGEN);
}
void Ccp::oneshot(bool tf){
    r.setbit(m_ccpx_con, ONESHOT, tf);
}
void Ccp::sync_altout(bool tf){
    r.setbit(m_ccpx_con, ALTSYNC, tf);
}
void Ccp::sync_src(SYNC e){
    r.clrbit(m_ccpx_con, SYNC_CLR<<SYNC_SHIFT);
    r.setbit(m_ccpx_con, e<<SYNC_SHIFT);
}
void Ccp::on(bool tf){
    r.setbit(m_ccpx_con, ON, tf);
}
void Ccp::stop_idle(bool tf){
    r.setbit(m_ccpx_con, SIDL, !tf);
}
void Ccp::stop_sleep(bool tf){
    r.setbit(m_ccpx_con, CCPSLP, !tf);
}
void Ccp::sync_tmr(bool tf){
    r.setbit(m_ccpx_con, TMRSYNC, tf);
}
void Ccp::clk_src(CLKSEL e){
    r.clrbit(m_ccpx_con, CLKSEL_CLR<<CLKSEL_SHIFT);
    r.setbit(m_ccpx_con, e<<CLKSEL_SHIFT);
}
void Ccp::tmr_prescale(TMRPS e){
    r.clrbit(m_ccpx_con, TMRPS_CLR<<TMRPS_SHIFT);
    r.setbit(m_ccpx_con, e<<TMRPS_SHIFT);
}
void Ccp::mode(MODE e){
    bool ison = r.anybit(m_ccpx_con, ON);
    on(false);
    r.clrbit(m_ccpx_con, MODE_CLR<<MODE_SHIFT);
    r.setbit(m_ccpx_con, e<<MODE_SHIFT);
    if(ison) on(true);
}

//ccpxcon2
void Ccp::out_sync(bool tf){
   r.setbit(m_ccpx_con+CCPXCON2, OENSYNC, tf);
}
void Ccp::out_pins(OUTPINS e){
   r.clrbit(m_ccpx_con+CCPXCON2, OCPINS_CLR<<OCPINS_SHIFT);
   r.setbit(m_ccpx_con+CCPXCON2, e<<OCPINS_SHIFT);
}
void Ccp::gate_mode(ICGSM e){
   r.clrbit(m_ccpx_con+CCPXCON2, ICGSM_CLR<<ICGSM_SHIFT);
   r.setbit(m_ccpx_con+CCPXCON2, e<<ICGSM_SHIFT);
}
void Ccp::out_aux(AUXOUT e){
   r.clrbit(m_ccpx_con+CCPXCON2, AUXOUT_CLR<<AUXOUT_SHIFT);
   r.setbit(m_ccpx_con+CCPXCON2, e<<AUXOUT_SHIFT);
}
void Ccp::cap_src(ICS e){
   r.clrbit(m_ccpx_con+CCPXCON2, ICS_CLR<<ICS_SHIFT);
   r.setbit(m_ccpx_con+CCPXCON2, e<<ICS_SHIFT);
}
void Ccp::pwm_restart(bool tf){
   r.setbit(m_ccpx_con+CCPXCON2, PWMRSEN, tf);
}
void Ccp::gate_auto(bool tf){
   r.setbit(m_ccpx_con+CCPXCON2, ASDGM, tf);
}
void Ccp::gate_now(bool tf){
   r.setbit(m_ccpx_con+CCPXCON2, SSDG, tf);
}
void Ccp::gate_autosrc(GATEAUTOSRC e){
   r.val((vbyte_ptr)m_ccpx_con+(CCPXCON2*4), e);
}

//ccpxcon3
void Ccp::out_trigwait(bool tf){
    r.setbit(m_ccpx_con+CCPXCON3, OETRIG, tf);
}
void Ccp::oneshot_extend(uint8_t v){
    r.clrbit(m_ccpx_con+CCPXCON3, OSCNT_CLR<<OSCNT_SHIFT);
    r.setbit(m_ccpx_con+CCPXCON3, (v&OSCNT_CLR)<<OSCNT_SHIFT);
}
void Ccp::out_mode(OUTM e){
    r.clrbit(m_ccpx_con+CCPXCON3, OUTM_CLR<<OUTM_SHIFT);
    r.setbit(m_ccpx_con+CCPXCON3, e<<OUTM_SHIFT);
}
void Ccp::polarity_ace(POLARITY e){
    r.setbit(m_ccpx_con+CCPXCON3, POLACE, e);
}
void Ccp::polarity_bdf(POLARITY e){
    r.setbit(m_ccpx_con+CCPXCON3, POLBDF, e);
}
void Ccp::shutdown_ace(SHUTDOWN e){
    r.clrbit(m_ccpx_con+CCPXCON3, PSSACE_CLR<<PSSACE_SHIFT);
    r.setbit(m_ccpx_con+CCPXCON3, e<<PSSACE_SHIFT);
}
void Ccp::shutdown_bdf(SHUTDOWN e){
    r.clrbit(m_ccpx_con+CCPXCON3, PSSBDF_CLR<<PSSBDF_SHIFT);
    r.setbit(m_ccpx_con+CCPXCON3, e<<PSSBDF_SHIFT);
}
void Ccp::dead_time(uint8_t v){
    r.clrbit(m_ccpx_con+CCPXCON3, DT_CLR<<DT_SHIFT);
    r.setbit(m_ccpx_con+CCPXCON3, (v&DT_CLR)<<DT_SHIFT);
}

//ccpxstat
bool Ccp::pr16_busy(){
    return r.anybit(m_ccpx_con+CCPXSTAT, PRLWIP);
}
bool Ccp::tmr32_busy(){
    return r.anybit(m_ccpx_con+CCPXSTAT, TMRHWIP);
}
bool Ccp::tmr16_busy(){
    return r.anybit(m_ccpx_con+CCPXSTAT, TMRLWIP);
}
bool Ccp::compb_busy(){
    return r.anybit(m_ccpx_con+CCPXSTAT, RBWIP);
}
bool Ccp::compa_busy(){
    return r.anybit(m_ccpx_con+CCPXSTAT, RAWIP);
}
void Ccp::gate_arm(){
     r.setbit(m_ccpx_con+CCPXSTAT, ICGARM);
}
bool Ccp::stat_trig(){
    return r.anybit(m_ccpx_con+CCPXSTAT, CCPTRIG);
}
void Ccp::trig_set(){
     r.setbit(m_ccpx_con+CCPXSTAT, TRSET);
}
void Ccp::trig_clr(){
     r.setbit(m_ccpx_con+CCPXSTAT, TRCLR);
}
bool Ccp::stat_shutdown(){
    return r.anybit(m_ccpx_con+CCPXSTAT, ASEVT);
}
bool Ccp::stat_secomp(){
    return r.anybit(m_ccpx_con+CCPXSTAT, SCEVT);
}
bool Ccp::stat_capdis(){
    return r.anybit(m_ccpx_con+CCPXSTAT, ICDIS);
}
bool Ccp::stat_oflow(){
    return r.anybit(m_ccpx_con+CCPXSTAT, ICOV);
}
bool Ccp::stat_bufany(){
    return r.anybit(m_ccpx_con+CCPXSTAT, ICBNE);
}

//ccpxtmr
void Ccp::tmr16(uint16_t v){
    r.val(m_ccpx_con+CCPXTMR, v);
}
void Ccp::tmr16h(uint16_t v){
    r.val((vbyte_ptr)m_ccpx_con+(CCPXTMR*4)+2, v);
}
void Ccp::tmr32(uint32_t v){
    r.val(m_ccpx_con+CCPXTMR, v);
}
uint16_t Ccp::tmr16(){
    return r.val16(m_ccpx_con+CCPXTMR);
}
uint16_t Ccp::tmr16h(){
    return r.val16((vbyte_ptr)m_ccpx_con+(CCPXTMR*4)+2);
}
uint32_t Ccp::tmr32(){
    return r.val(m_ccpx_con+CCPXTMR);
}

//ccpxpr
void Ccp::pr16(uint16_t v){
    r.val(m_ccpx_con+CCPXPR, v);
}
void Ccp::pr16h(uint16_t v){
    r.val((vbyte_ptr)m_ccpx_con+(CCPXPR*4)+2, v);
}
void Ccp::pr32(uint32_t v){
    r.val(m_ccpx_con+CCPXPR, v);
}
uint16_t Ccp::pr16(){
    return r.val16(m_ccpx_con+CCPXPR);
}
uint16_t Ccp::pr16h(){
    return r.val16((vbyte_ptr)m_ccpx_con+(CCPXPR*4)+2);
}
uint32_t Ccp::pr32(){
    return r.val(m_ccpx_con+CCPXPR);
}

//ccpxra, ccpxrb
void Ccp::compa(uint16_t v){
    r.val(m_ccpx_con+CCPXRA, v);
}
void Ccp::compb(uint16_t v){
    r.val(m_ccpx_con+CCPXRB, v);
}
void Ccp::comp32(uint32_t v){
    compb(v>>16);
    compa(v);
}
uint16_t Ccp::compa(){
    return r.val16(m_ccpx_con+CCPXRA);
}
uint16_t Ccp::compb(){
    return r.val16(m_ccpx_con+CCPXRB);
}
uint32_t Ccp::comp32(){
    return compb()<<16 | compa();
}

//ccpxbuf
uint16_t Ccp::buf16(){
    return r.val16(m_ccpx_con+CCPXBUF);
}
uint32_t Ccp::buf32(){
    return r.val(m_ccpx_con+CCPXBUF);
}
