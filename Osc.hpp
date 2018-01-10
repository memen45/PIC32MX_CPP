#pragma once

/*=============================================================================
 Oscillator - set cpu clock source, speed

 any fucntion that depends on cpu frequency, can call sysclk() to get current
 cpu frequency in Hz, if not already calculated it will be calculated when
 called or when cpu source/speed is changed, if external clock/crystal is
 used then frequency is calculated using sosc or lprc UNLESS m_extosc_freq
 is set in this file by user to anything other than 0

 config bits will need to enable clock source switching if changing clock
 sources OR changing PLL mul/div values
 #pragma config FCKSM = CSECMD

 just set frc as clock source in config bits to prevent pll problems after
 a reset (where MUL is too high, and reset will seem to lock up chip)
 #pragma config FNOSC = FRCDIV

 use Osc::sysclk() to retrieve sysclock

 any peripheral specifying SOSC as clock source, will also use
 Osc::sosc(true) to make sure sosc is turned on, but no check if SOSC is
 present (assume if specified, caller must know its available), if peripheral
 like rtcc needs a clock but is not specified, then peripheral will call
 sosc() to check if its on already, if on assume its available

 if using sosc, just call Osc::sosc(true) early in code (may already be fused
 on), so any peripheral needing it can use it

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

    static void         frc_div     (DIVS);     //set
    static DIVS         frc_div     ();         //get
    static CNOSC        clk_src     ();         //get current osc source
    static void         clk_src     (CNOSC);    //start switch to nosc
    static void         clk_lock    ();         //lock nosc/oswen until reset
    static void         sleep       (bool);     //sleep enable
    static bool         clk_bad     ();         //clock failed?
    static void         sosc        (bool);     //sosc enable
    static bool         sosc        ();         //sosc enabled?
                                                //if enabled, assume its there

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //spllcon

    enum PLLMUL : uint8_t {
        MUL2 = 0, MUL3, MUL4, MUL6, MUL8, MUL12, MUL24
    };

    enum PLLSRC : bool { EXT, FRC };

    static DIVS         pll_div     ();         //get pll divider
    static PLLMUL       pll_mul     ();         //get pll multiplier
    static PLLSRC       pll_src     ();         //get pll src
    private:
    static void         pll_src     (PLLSRC);   //set pll src
    public:
    static void         pll_set     (PLLMUL, DIVS, PLLSRC = FRC);
                                                //set pll mul/div, pll src

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //refo1con, refo1trim

    enum ROSEL : uint8_t {
        RSYSCLK = 0, RPOSC = 2, RFRC, RLPRC, RSOSC, RPLLVCO = 7
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
    static uint32_t     refoclk     ();         //get refo in clk freq
    static void         refo_freq   (uint32_t); //set refo frequency
    static uint32_t     refo_freq   ();         //get refo frequency

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
    static uint32_t     extclk       ();        //get ext clock
    static uint32_t     frcclk       ();        //get frc clock


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;                               //for static class access
    static Syskey sk;                           //I like better than ::
    static Irq ir;

    static uint32_t m_sysclk;                    //store calculated cpu freq
    static uint32_t m_refoclk;                   //store calculated refo in clk
    static uint32_t m_extclk;
    static uint32_t m_refo_freq;                 //store current refo frequency

    static const uint32_t m_frcosc_freq = 8000000;
    static const uint32_t m_extosc_freq = 0;    //set if want exact ext freq
    static const uint8_t m_mul_lookup[7];

    enum IDSTAT { IRQ = 1, DMA = 2 };           //irq,dma status

    static IDSTAT       unlock_irq  ();         //unlock- irq's off, dma susp
    static void         lock_irq    (IDSTAT);   //lock-, restore irq, dma


    enum {
        OSCCON = 0xBF802680,
            CLKLOCK = 1<<7,
            SLPEN = 1<<4,
            CF = 1<<3,
            SOSCEN = 1<<1,
            OSWEN = 1<<0,
        SPLLCON = 0xBF8026A0,
            PLLICLK = 1<<7,
        REFO1CON = 0xBF802720,
            ON = 1<<15,
            SIDL = 1<<13,
            OE = 1<<12,
            RSLP = 1<<11,
            DIVSWEN = 1<<9,
            ACTIVE = 1<<8,
        REFO1TRIM = 0xBF802730,
        CLKSTAT = 0xBF802770,
        OSCTUN = 0xBF802880,
            /* ON = 1<<15, SIDL = 1<<13, from refo1con*/
            SRC = 1<<12,
            LOCK = 1<<11,
            POL = 1<<10,
            ORNG = 1<<9,
            ORPOL = 1<<8,
        DMACON = 0xBF808900, //need until dma class written
            DMASUSP = 1<<12
    };

};

/*=============================================================================
 all functions inline
=============================================================================*/
#include "Timer1.hpp"
#include "Cp0.hpp"

uint32_t Osc::m_sysclk = 0;
uint32_t Osc::m_refoclk = 0;
uint32_t Osc::m_extclk = 0;
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
//system lock enable, restore previous irq and dma status
void Osc::lock_irq(IDSTAT idstat){
    sk.lock();
    if((uint8_t)idstat & DMA) r.clrbit(DMACON, DMASUSP);
    if((uint8_t)idstat & IRQ) ir.enable_all();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//osccon

void Osc::frc_div(DIVS e){
    sk.unlock();
    r.val(OSCCON+3, e);
    sk.lock();
}
auto Osc::frc_div() -> DIVS {
    return (DIVS)r.val8(OSCCON+3);
}
auto Osc::clk_src() -> CNOSC {
    return (CNOSC)(r.val8(OSCCON+1)>>4);
}
void Osc::clk_src(CNOSC e){
    IDSTAT irstat = unlock_irq();
    r.val(OSCCON+1, e);
    r.setbit(OSCCON, OSWEN);
    while(r.anybit(OSCCON, OSWEN));
    lock_irq(irstat);
    m_sysclk = 0;
    sysclk();
}
void Osc::clk_lock(){
    r.setbit(OSCCON, CLKLOCK);
}
void Osc::sleep(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SLPEN, tf);
    sk.lock();
}
bool Osc::clk_bad(){
    return r.anybit(OSCCON, CF);
}
void Osc::sosc(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SOSCEN, tf);
    sk.lock();
}
bool Osc::sosc(){
    r.anybit(OSCCON, SOSCEN);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//spllcon

auto Osc::pll_div() -> DIVS {
    return (DIVS)r.val8(SPLLCON+3);
}
auto Osc::pll_mul() -> PLLMUL {
    return (PLLMUL)r.val8(SPLLCON+2);
}
auto Osc::pll_src() -> PLLSRC {
    return (PLLSRC)r.anybit(SPLLCON, PLLICLK);
}
//private, use pll_set to change src
void Osc::pll_src(PLLSRC e){
    r.setbit(SPLLCON, PLLICLK, e);
    m_refoclk = 0;  //recalculate refo clock
    refoclk();     //as input now may be different
}
//set SPLL as clock source with specified mul/div
//PLLSRC default is FRC
void Osc::pll_set(PLLMUL m, DIVS d, PLLSRC frc){
    IDSTAT irstat  = unlock_irq();
    //need to switch from SPLL to something else
    //switch to frc (hardware does nothing if already frc)
    clk_src(FRCDIV);
    //set new pll vals
    r.val(SPLLCON+3, d);
    r.val(SPLLCON+2, m);
    //pll select
    pll_src(frc); //do after m, so refoclk() sees new m value
    //source to SPLL
    clk_src(SPLL);
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
    refoclk(); //calculate if needed
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
    bool ison = r.anybit(REFO1CON, ON);
    refo_off();
    if(e == RSOSC) sosc(true);
    r.val(REFO1CON, e);
    if(ison) refo_on();
    m_refoclk = 0;
    refoclk();
}
//called by refo_src(), refo_on(), refo_freq()
uint32_t Osc::refoclk(){
    if(m_refoclk) return m_refoclk; //previously calculated
    switch(r.val8(REFO1CON)){
        case RSYSCLK:   m_refoclk = sysclk();       break;
        case RPOSC:     m_refoclk = extclk();       break;
        case RFRC:      m_refoclk = m_frcosc_freq;  break;
        case RLPRC:     m_refoclk = 32125;          break;
        case RSOSC:     m_refoclk = 32768;          break;
        case RPLLVCO:   m_refoclk = vcoclk();       break;
        //should not get anything else, could set clk src
        //to frc/pll if think could ever get here
    }
    //if previously set refo freq, do again with possibly new src
    if(m_refo_freq) refo_freq(m_refo_freq);
    return m_refoclk;
}
//also called when pll input or pll mul changed
void Osc::refo_freq(uint32_t v){
    uint32_t m, n;
    m_refo_freq = 0; //prevent call back to here from refoclk()
    refoclk(); //if not calculated already
    m = (m_refoclk << 8) / v;
    n =  m >> 9;
    m &= 0x1ff;
    refo_div(n);
    refo_trim(m);
    refo_divsw();
    m_refo_freq = v;
}
uint32_t Osc::refo_freq(){
    return m_refo_freq;
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
    if(e == TSOSC) sosc(true);
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
    if(m_sysclk) return m_sysclk;           //already have it
    CNOSC s = clk_src();
    switch(s){
        case LPRC: m_sysclk = 32000; break; //+/-20% = 25600 - 38400
        case SOSC: m_sysclk = 32768; break;
        case POSC: m_sysclk = extclk(); break;
        case SPLL: {
            uint32_t f = pll_src() == FRC ? m_frcosc_freq : extclk();
            uint8_t m = (uint8_t)pll_mul();  //2 3 4 6 8 12 24
            m = m_mul_lookup[m];
            uint8_t d = (uint8_t)pll_div();
            if(d == 7) d = 8;               //adjust DIV256
            m_sysclk = (f*m)>>d;
            break;
        }
        case FRCDIV: {
            uint8_t n = (uint8_t)frc_div();
            if(n == 7) n = 8;               //adjust DIV256
            m_sysclk = m_frcosc_freq>>n;
            break;
        }
        default:
            m_sysclk = m_frcosc_freq;      //should not be able to get here
            break;
    }
    return m_sysclk;
}

//input to refo if PLLVCO is source
uint32_t Osc::vcoclk(){
    uint32_t f = 0;
    f = pll_src() == FRC ? m_frcosc_freq : extclk() ;
    return m_mul_lookup[ pll_mul() ] * f;
}

//get ext clock freq, using sosc if available, or lprc
//and cp0 counter to calculate
//OR if user defined m_extosc_freq, return that
//irq's disabled if calculation needed (re-using unlock_irq)
//will only run once- will assume an ext clock will not change
uint32_t Osc::extclk(){
    if(m_extosc_freq) return m_extosc_freq;
    if(m_extclk) return m_extclk;
    Timer1 t1; Cp0 cp0;
    IDSTAT irstat  = unlock_irq();
   //backup timer1 (we want it, but will give it back)
    uint16_t t1conbak = *(volatile uint16_t*)t1.T1CON;
    *(volatile uint16_t*)t1.T1CON = 0; //stop
    uint16_t pr1bak = t1.period();
    uint32_t t1bak = t1.timer();
    //reset
    t1.period(0xFFFF);
    t1.prescale(t1.PS1);
    t1.timer(0);
    //if sosc enabled, assume it is there
    if(sosc()) t1.clk_src(t1.SOSC);
    else t1.clk_src(t1.LPRC);
   //start timer1, get cp0 count
    t1.on(true);
    uint32_t c = cp0.count();
   //wait for ~1/4sec, get cp0 total count
    while(t1.timer() < 8192);
    c = cp0.count() - c;
   //cp0 runs at sysclk/2, so x2 x4 = x8 or <<3
    c <<= 3;
   //resolution only to 0.1Mhz
    c /= 100000;
    c *= 100000;
    m_extclk = c;
   //restore timer1
    t1.on(false);
    t1.timer(t1bak);
    t1.period(pr1bak);
    *(volatile uint32_t*)t1.T1CON = t1conbak;
    lock_irq(irstat);
    return m_extclk;
}

uint32_t Osc::frcclk(){
    return m_frcosc_freq;
}

/*

func        reg         calls                   recalculate
-----------------------------------------------------------------------
frc_div()   FRCDIVv     sysclk()                [m_sysclk]
clk_src()   NOSC        sysclk()                [m_sysclk]
pll_src()   PLLICLK     refo_clk()              [m_refoclk]
pll_set()   PLLxxxx     pll_src(),clk_src()     [m_refoclk][m_sysclk]
refo_src()  ROSEL       refo_clk()              [m_refoclk]



refo_clk()  recalculates m_refoclk if set to 0
              else returns m_refoclk
            re-sets refo freq if m_refo_freq not 0
            returns m_refoclk

sysclk()    recalculates m_sysclk if set to 0
                else returns m_sysclk
            returns m_sysclk

//used by anyone to get sysclock
Osc::sysclk()
//used by anyone to get refo frequency
Osc::refo_freq()
//only needed by Osc
Osc::refoclk()
Osc::extclk()
Osc::vcoclk()


*/