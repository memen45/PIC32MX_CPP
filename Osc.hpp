#pragma once

/*=============================================================================
 Oscillator - set cpu clock source, speed

 any fucntion that depends on cpu frequency, can call sysclk() to get current
 cpu frequency in Hz, if not already calculated it will be calculated when
 called or when cpu source/speed is changed, if unable to calculate (if ext
 crystal) then the default frequency is returned

 config bits will need to enable clock source switching if changing clock
 sources OR changing PLL mul/div values

 after software reset AND spll is clock selected in config bits, will lock
 up when trying to switch to spll in the pllset() function, so just set
 frc as clock source in config bits
 #pragma config FNOSC = FRCDIV

=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"
#include "Irq.hpp"

struct Osc {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //osccon

    enum DIVS : uint8_t { //upper byte of osccon, spllcon
        DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV256
    };
    enum CNOSC : uint8_t { //cosc/nosc in second byte of osccon
         FRCDIV = 0, SPLL, POSC, SOSC = 4, LPRC
    };

    static void         frcdiv      (DIVS);     //set
    static DIVS         frcdiv      ();         //get
    static CNOSC        clksrc      ();         //get current osc source
    static void         clksrc      (CNOSC);    //start switch to nosc
    static void         clklock     ();         //lock nosc/oswen until reset
    static void         sleep       (bool);     //sleep enable
    static bool         clkbad      ();         //clock failed?
    static void         sosc        (bool);     //sosc enable

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spllcon

    enum PLLMUL : uint8_t {
        MUL2 = 0, MUL3, MUL4, MUL6, MUL8, MUL12, MUL24
    };

    enum PLLSRC : bool { EXT, FRC };

    static DIVS         plldiv      ();         //get pll divider
    static PLLMUL       pllmul      ();         //get pll multiplier
    static PLLSRC       pllsrc      ();         //get pll src
    static void         pllsrc      (PLLSRC);   //set pll src
    static void         pllset      (PLLMUL, DIVS, PLLSRC = FRC);
                                                //set pll mul/div, pll src

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //refo1con, refo1trim

    enum ROSEL : uint8_t {
        RSYSCLK = 0, RPOSC = 2, RFRC = 3, RLPRC = 4, RSOSC = 5, RPLLVCO = 7
    };

    static void         refo_div    (uint16_t); //divisor value
    static void         refo_trim   (uint16_t); //trim value
    static void         refo_on     ();         //refo on
    static void         refo_on     (ROSEL);    //refo on, src sel
    static void         refo_off    ();         //refo off
    static void         refo_idle   (bool);     //true = stop in idle mode
    static void         refo_out    (bool);     //true = clk out to REFO1 pin
    static void         refo_sleep  (bool);     //true = run in sleep
    static void         refo_divsw  ();         //do divider switch
    static bool         refo_active ();         //true = active
    static void         refo_src    (ROSEL);    //clk source select
    static uint32_t     refo_clk    ();         //get refo in clk freq
    static void         refo_freq   (uint32_t); //set refo frequency

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //clkstat

    enum CLKRDY : uint8_t {
        FRCRDY = 0, SPLLDIVRDY, POSCRDY,
        SOSCRDY = 4, LPRCRDY, USBRDY, SPLLRDY
    };

    static bool         ready       (CLKRDY);   //clock ready?

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //osctun

    enum TUNSRC : bool { TSOSC, TUSB };

    static void         tun_auto    (bool);     //osc tune on
    static void         tun_idle    (bool);     //stop in idle
    static void         tun_src     (TUNSRC);   //src, 0=sosc 1=usb
    static bool         tun_lock    ();         //lock status
    static void         tun_lpol    (bool);     //irq polarity, 1=0 0=1
    static bool         tun_rng     ();         //out of range status
    static void         tun_rpol    (bool);     //range polarity, 1=0 0=1
    static void         tun_val     (int8_t);   //tune value 6bits, -32 to 31
    static int8_t       tun_val     ();         //get tune value



    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //misc

    static uint32_t     sysclk       ();        //get cpu sysclk
    static uint32_t     vcoclk       ();        //get pll vco clock


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;                               //for static class access
    static Syskey sk;                           //I like better than ::
    static Irq ir;

    static uint32_t m_sysclk;                    //store calculated cpu freq
    static uint32_t m_refoclk;                   //store calculated refo in clk
    static uint32_t m_refo_freq;                 //store current refo frequency

    static const uint32_t m_default_freq = 24000000;
    static const uint32_t m_frcosc_freq = 8000000;
    static const uint32_t m_extosc_freq = 12000000; //use until we can calculate
    static const uint8_t m_mul_lookup[7];

    enum IDSTAT { IRQ = 1, DMA = 2 };           //irq,dma status

    static IDSTAT       unlock_irq  ();         //unlock- irq's off, dma susp
    static void         lock_irq    (IDSTAT);   //lock-, restore irq, dma


    enum {
        OSCCON = 0xBF802680,
            CLKLOCK = 1<<7, SLPEN = 1<<4, CF = 1<<3,
            SOSCEN = 1<<1, OSWEN = 1<<0,
        SPLLCON = 0xBF8026A0,
            PLLICLK = 1<<7,
        REFO1CON = 0xBF802720,
            ON = 1<<15, SIDL = 1<<13, OE = 1<<12, RSLP = 1<<11,
            DIVSWEN = 1<<9, ACTIVE = 1<<8,
        REFO1TRIM = 0xBF802730,
        CLKSTAT = 0xBF802770,
        OSCTUN = 0xBF802880,
            /* ON = 1<<15, SIDL = 1<<13, from refo1con*/
            SRC = 1<<12, LOCK = 1<<11, POL = 1<<10,
            ORNG = 1<<9, ORPOL = 1<<8,
        DMACON = 0xBF808900, //need until dma class written
            DMASUSP = 1<<12
    };

};

/*=============================================================================
 all functions inline
=============================================================================*/
uint32_t Osc::m_sysclk = 0;
uint32_t Osc::m_refoclk = 0;
uint32_t Osc::m_refo_freq = 0;
const uint8_t Osc::m_mul_lookup[] = {2, 3, 4, 6, 8, 12, 24};

//some functions need irq disabled, others can use
//Syskey::unlock/lock directly

//system unlock for register access, w/irq,dma disable
auto Osc::unlock_irq() -> IDSTAT {
    uint8_t idstat = ir.all_ison();
    ir.disable_all();
    idstat |= r.anybit(DMACON, DMASUSP)<<1;
    r.setbit(DMACON, DMASUSP);
    sk.unlock();
    return (IDSTAT)idstat;
}
//system lock enable, restore previous irq status
void Osc::lock_irq(IDSTAT idstat){
    sk.lock();
    if((uint8_t)idstat & DMA) r.clrbit(DMACON, DMASUSP);
    if((uint8_t)idstat & IRQ) ir.enable_all();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//osccon

void Osc::frcdiv(DIVS e){
    sk.unlock();
    r.val(OSCCON+3, e);
    sk.lock();
}
auto Osc::frcdiv() -> DIVS {
    return (DIVS)r.val8(OSCCON+3);
}
auto Osc::clksrc() -> CNOSC {
    return (CNOSC)(r.val8(OSCCON+1)>>4);
}
void Osc::clksrc(CNOSC e){
    IDSTAT irstat = unlock_irq();
    r.val(OSCCON+1, e);
    r.setbit(OSCCON, OSWEN);
    while(r.anybit(OSCCON, OSWEN));
    lock_irq(irstat);
    m_sysclk = 0;
    sysclk();
}
void Osc::clklock(){
    r.setbit(OSCCON, CLKLOCK);
}
void Osc::sleep(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SLPEN, tf);
    sk.lock();
}
bool Osc::clkbad(){
    return r.anybit(OSCCON, CF);
}
void Osc::sosc(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SOSCEN, tf);
    sk.unlock();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spllcon

auto Osc::plldiv() -> DIVS {
    return (DIVS)r.val8(SPLLCON+3);
}
auto Osc::pllmul() -> PLLMUL {
    return (PLLMUL)r.val8(SPLLCON+2);
}
auto Osc::pllsrc() -> PLLSRC {
    return (PLLSRC)r.anybit(SPLLCON, PLLICLK);
}
void Osc::pllsrc(PLLSRC e){
    r.setbit(SPLLCON, PLLICLK, e);
    m_refoclk = 0;  //recalculate refo clock
    refo_clk();     //as input now may be different
}
//set SPLL as clock source with specified mul/div
//PLLSRC default is FRC
void Osc::pllset(PLLMUL m, DIVS d, PLLSRC frc){
    IDSTAT irstat  = unlock_irq();
    //need to switch from SPLL to something else
    //switch to frc (hardware does nothing if already frc)
    clksrc(FRCDIV);
    //set new pll vals
    r.val(SPLLCON+3, d);
    r.val(SPLLCON+2, m);
    //pll select
    pllsrc(frc); //do after m, so refo_clk() sees new m value
    //source to SPLL
    clksrc(SPLL);
    lock_irq(irstat);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//refo1con, refo1trim

void Osc::refo_div(uint16_t v){
    r.val(REFO1CON, v);
}
void Osc::refo_trim(uint16_t v){
    r.val(REFO1TRIM+2, v<<7);
}
void Osc::refo_on(){
    refo_clk(); //calculate if needed
    r.setbit(REFO1CON, ON);
    while(refo_active() == 0);
}
void Osc::refo_on(ROSEL e){
    refo_src(e);
    refo_on();
}
void Osc::refo_off(){
    r.clrbit(REFO1CON, ON);
    while(refo_active());
}
void Osc::refo_idle(bool tf){
    r.setbit(REFO1CON, SIDL, tf);
}
void Osc::refo_out(bool tf){
    r.setbit(REFO1CON, OE, tf);
}
void Osc::refo_sleep(bool tf){
    r.setbit(REFO1CON, RSLP, tf);
}
void Osc::refo_divsw(){
    r.setbit(REFO1CON, DIVSWEN);
    while(r.anybit(REFO1CON, DIVSWEN));
}
bool Osc::refo_active(){
    return r.anybit(REFO1CON, ACTIVE);
}
//anytime source set, get new m_refoclk
//force recalculate by setting m_refoclk to 0
void Osc::refo_src(ROSEL e){
    refo_off();
    r.val(REFO1CON, e);
    m_refoclk = 0;
    refo_clk();
}
//called by refo_src(), refo_on(), refo_freq()
uint32_t Osc::refo_clk(){
    if(m_refoclk) return m_refoclk; //previously calculated
    switch(r.val8(REFO1CON)){
        case RSYSCLK:   m_refoclk = sysclk();       break;
        case RPOSC:     m_refoclk = m_extosc_freq;  break;
        case RFRC:      m_refoclk = m_frcosc_freq;  break;
        case RLPRC:     m_refoclk = 32125;          break;
        case RSOSC:     m_refoclk = 32768;          break;
        case RPLLVCO:   m_refoclk = vcoclk();       break;
        //should not get anything else, but if do, use sysclk
        default:        m_refoclk = sysclk();       break;
    }
    //if previously set refo freq, do again with psooibly new src
    if(m_refo_freq) refo_freq(m_refo_freq);
    return m_refoclk;
}
//also called when pll input or pll mul changed
void Osc::refo_freq(uint32_t v){
    uint32_t m, n;
    m_refo_freq = 0; //prevent call back to here from refo_clk()
    refo_clk(); //if not calculated already
    m = (m_refoclk << 8) / v;
    n =  m >> 9;
    m &= 0x1ff;
    refo_div(n);
    refo_trim(m);
    refo_divsw();
    m_refo_freq = v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//clkstat

bool Osc::ready(CLKRDY e){
    return r.anybit(CLKSTAT, e);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//osctun

void Osc::tun_auto(bool tf){
    sk.unlock();
    r.setbit(OSCTUN, ON, tf);
    sk.lock();
}
void Osc::tun_idle(bool tf){
    sk.unlock();
    r.setbit(OSCTUN, SIDL, tf);
    sk.lock();
}
void Osc::tun_src(TUNSRC e){
    sk.unlock();
    r.setbit(OSCTUN, SRC, e);
    sk.lock();
}
bool Osc::tun_lock(){
    return r.anybit(OSCTUN, LOCK);
}
void Osc::tun_lpol(bool tf){
    sk.unlock();
    r.setbit(OSCTUN, POL, !tf);
    sk.lock();
}
bool Osc::tun_rng(){
    return r.anybit(OSCTUN, ORNG);
}
void Osc::tun_rpol(bool tf){
    sk.unlock();
    r.setbit(OSCTUN, ORPOL, !tf);
    sk.lock();
}
void Osc::tun_val(int8_t v ){
    //linit -32 to +31
    if(v > 31) v = 31;
    if(v < -32) v = -32;
    sk.unlock();
    r.val(OSCTUN, v);
    sk.lock();
}
int8_t Osc::tun_val(){
    int8_t v = r.val8(OSCTUN);
    if(v > 31) v |= 0xc0; //is negative, sign extend to 8bits
    return v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//misc

uint32_t Osc::sysclk(){
    if(m_sysclk) return m_sysclk;    //already have it
    m_sysclk = m_default_freq;       //assume default if cannot get
    CNOSC s = clksrc();
    switch(s){
        case LPRC: m_sysclk = 31250; break;
        case SOSC: m_sysclk = 32768; break;
        case POSC: m_sysclk = m_extosc_freq; break;
        case SPLL: {
            uint32_t f = pllsrc() == FRC ? m_frcosc_freq : m_extosc_freq;
            uint8_t m = (uint8_t)pllmul();  //2 3 4 6 8 12 24
            m = m_mul_lookup[m];
            uint8_t d = (uint8_t)plldiv();
            if(d == 7) d = 8;               //adjust DIV256
            m_sysclk = (f*m)>>d;
            break;
        }
        case FRCDIV: {
            uint8_t n = (uint8_t)frcdiv();
            if(n == 7) n = 8;               //adjust DIV256
            m_sysclk = m_frcosc_freq>>n;
            break;
        }
        default:
            break;
    }
    return m_sysclk;
}

//input to refo if PLLVCO is source
uint32_t Osc::vcoclk(){
    uint32_t f = 0;
    f = pllsrc() == FRC ? m_frcosc_freq : m_extosc_freq ;
    return m_mul_lookup[ pllmul() ] * f;
}
