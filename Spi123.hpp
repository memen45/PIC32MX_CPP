#pragma once

#include <cstdint>
#include "Reg.hpp"
#include "Osc.hpp"

/*=============================================================================
 SPI 1/2/3 functions
=============================================================================*/

struct Spi123  {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //instantiate Comp123 with comparator number
    enum SPIX { SPI1 = 0, SPI2, SPI3 };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    constexpr Spi123(SPIX);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spixcon

    enum {
        FRMEN = 1<<31,
        FRMSYNC = 1<<30,        FSLAVE = 1,     FMASTER = 0,
        FRMPOL = 1<<29,         POLH = 1,       POLL = 0,
        MSSEN = 1<<28,
        FRMSYPW = 1<<27,        PWCHAR = 1,     PWCLK = 0,
        FRMCNT_SHIFT = 24, FRMCNT_CLR = 7,
        MCLKSEL = 1<<23,        REFOCLK = 1,    PBCLK = 0,
        SPIFE = 1<<17,          ATBCLK = 1,     B4BCLK = 0,
        ENHBUF = 1<<16,
        ON = 1<<15,
        MODE_SHIFT = 10, MODE_CLR = 3,
        SMP = 1<<9,             SMPEND = 1,     SMPMID = 0,
        CKE = 1<<8,             CKETRAIL = 1,   CKELEAD = 0,
        SSEN = 1<<7,
        CKP = 1<<6,             CKPHL = 1,      CKPLH = 0,
        MSTEN = 1<<5,
        STXISEL_SHIFT = 2, STXISEL_CLR = 3,
        SRXISEL_SHIFT = 0, SRXISEL_CLR = 3
    };

    enum FRMCNT { CNT1 = 0, CNT2, CNT4, CNT8, CNT16, CNT32 };
    enum MODE {
        MODE8 = 0, MODE16, MODE32,
        MODE1616 = 0, MODE1632, MODE3232, MODE2432
    };
    enum TXIRQ { TDONE = 0, TEMPTY, THALF, TNOTFULL };
    enum RXIRQ { REMPTY = 0, RANY, RHALF, RFULL };

    void            frame           (bool);             //framed support
    void            frame_dir       (bool);             //frame sync pulse dir
    void            frame_pol       (bool);             //frame sync polarity
    void            slave_sel       (bool);             //slave select enable
    void            frame_pwidth    (bool);             //1=1char, 0=1clk
    void            frame_count     (FRMCNT);           //frame sync counter
    void            clk_sel         (bool);             //set clock source
    bool            clk_sel         ();                 //get clock source
    void            frame_edge      (bool);             //frame sunc edge sel
    void            enhanced        (bool);             //enhanced buffer mode
    void            on              (bool);             //spi on/off
    void            mode            (MODE);             //set spi mode
    void            phase           (bool);             //sample phase bit
    void            clk_edge        (bool);             //clk edge sel
    void            ss              (bool);             //slave select enable
    void            clk_pol         (bool);             //clock polarity
    void            master          (bool);             //master mode
    void            tx_irq          (TXIRQ);            //tx irq mode
    void            rx_irq          (RXIRQ);            //rx irq mode

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spixstat

    enum {
        FRMERR = 1<<12, SPIBUSY = 1<<11, SPITUR = 1<<8, SRMT = 1<<7,
        SPIROV = 1<<6, SPIRBE = 1<<5, SPITBE = 1<<3,
        SPITBF = 1<<1, SPIRBF = 1<<0
    };

    uint8_t         stat_rxcount    ();                 //enhanced rx buf count
    uint8_t         stat_txcount    ();                 //enhanced tx buf count
    bool            stat_frame      ();                 //framing error
    void            stat_frame      (bool);             //framing error clear
    bool            stat_busy       ();                 //spi busy
    bool            stat_txurun     ();                 //tx underrun error
    bool            stat_sremty     ();                 //shift reg empty
    bool            stat_oflow      ();                 //rx overflow
    void            stat_oflow      (bool);             //rx overflow clear
    bool            stat_rxemty     ();                 //rx empty
    bool            stat_txemty     ();                 //tx empty
    bool            stat_txfull     ();                 //tx full
    bool            stat_rxfull     ();                 //rx full

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spixbuf

    void            buf             (uint32_t);         //set buf 8/16/32bit
    uint32_t        buf             ();                 //get buf 8/16/32bit

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spixbrg

    void            baud            (uint16_t);         //set baud
    void            freq            (uint32_t);         //set frequency
    uint32_t        freq            ();                 //get frequency

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spixcon2

    enum {
        SPISGNEXT = 1<<15, FRMERREN = 1<<12, SPIROVEN = 1<<11,
        SPITUREN = 1<<10, IGNROV = 1<<9, IGNTUR = 1<<8,
        AUDEN = 1<<7, AUDOMONO = 1<<3
    };

    enum AUDMOD : uint8_t { I2S = 0, LEFT, RIGHT, PCMDSP };

    void            sign_ext        (bool);             //rx sign extend
    void            irq_frmerr      (bool);             //frame error irq
    void            irq_oflow       (bool);             //overflow error irq
    void            irq_urun        (bool);             //underrun err irq
    void            ign_oflow       (bool);             //ignore overflow
    void            ign_urun        (bool);             //ignore underrun
    void            audio           (bool);             //audio mode
    void            mono            (bool);             //audio mono
    void            audio_mode      (AUDMOD);           //audio mode


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    Reg r;
    Osc osc;

    enum {
        SPI1CON = 0xBF808100, SPIX_SPACING = 0x40, //spacing in words
    };

    volatile uint32_t* m_spixcon;
    volatile uint32_t* m_spixstat;
    volatile uint32_t& m_spixbuf;                   //use reference
    volatile uint32_t& m_spixbrg;                   //use reference
    volatile uint32_t* m_spixcon2;

    uint32_t m_spix_freq;                           //set to actual spi freq
};


/*=============================================================================
 all functions inline
=============================================================================*/
constexpr Spi123::Spi123(SPIX e)
    : m_spixcon((volatile uint32_t*)SPI1CON+(e*SPIX_SPACING)),
      m_spixstat((volatile uint32_t*)SPI1CON+(e*SPIX_SPACING)+4),
      m_spixbuf(*((volatile uint32_t*)SPI1CON+(e*SPIX_SPACING)+8)),
      m_spixbrg(*((volatile uint32_t*)SPI1CON+(e*SPIX_SPACING)+12)),
      m_spixcon2((volatile uint32_t*)SPI1CON+(e*SPIX_SPACING)+16),
      m_spix_freq(0)
{}

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
//spixbuf
void Spi123::buf(uint32_t v){
    m_spixbuf = v; //m_spibuf&
}
uint32_t Spi123::buf(){
    return m_spixbuf; //m_spibuf&
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spixbrg

void Spi123::baud(uint16_t v){
    m_spixbrg = v; //m_spibrg&
}
//set frequency
void Spi123::freq(uint32_t v){
    uint32_t clk;
    if(clk_sel() == REFOCLK) clk = osc.refo_freq();
    else clk = osc.sysclk();
    uint16_t brg = (2 * v / clk) - 1;
    brg &= 0x1ff;
    baud(brg);
    freq();
}
//get actual frequency
//called by clk_sel(), freq(uint32_t)
uint32_t Spi123::freq(){
    uint32_t clk;
    if(clk_sel() == REFOCLK) clk = osc.refo_freq();
    else clk = osc.sysclk();
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