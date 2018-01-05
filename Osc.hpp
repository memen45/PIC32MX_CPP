#pragma once

/*=============================================================================
 Oscillator - set cpu clock source, speed

 any fucntion that depends on cpu frequency, can call speed() to get current
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

    static DIVS         plldiv      ();         //get pll divider
    static PLLMUL       pllmul      ();         //get pll multiplier
    static bool         pllfrc      ();         //true=pll src frc, false=posc
    static void         pllfrc      (bool);     //true=pll src frc, false=posc
    static void         pllset      (PLLMUL, DIVS, bool = true);
                                                //set pll mul/div, true=frc

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //refo1con

    enum ROSEL : uint8_t {
        RSYSCLK = 0, RPOSC = 2, RFRC = 3, RLPRC = 4, RSOSC = 5, RPLLVCO = 7
    };

    static void         refo_div    (uint16_t); //divisor value
    static void         refo        (bool);     //true = on
    static void         refo_idle   (bool);     //true = stop in idle mode
    static void         refo_out    (bool);     //true = clk out to REFO1 pin
    static void         refo_sleep  (bool);     //true = run in sleep
    static void         refo_divsw  (bool);     //true = divider switch enable
    static bool         refo_divsw  ();         //false = divider switch done
    static bool         refo_active ();         //true = active
    static void         refo_src    (ROSEL);    //clk source select


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //refo1trim




    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //clkstat

    enum CLKRDY : uint8_t {
        FRCRDY = 0, SPLLDIVRDY, POSCRDY,
        SOSCRDY = 4, LPRCRDY, USBRDY, SPLLRDY
    };

    static bool         ready       (CLKRDY);   //clock ready?

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //osctun


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //misc

    static uint32_t     speed       ();         //get cpu speed


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;                               //for static class access
    static Syskey sk;                           //I like better than ::
    static Irq ir;

    static uint32_t m_speed;                    //store calculated cpu freq

    static const uint32_t m_default_speed = 24000000;

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

        DMACON = 0xBF808900, //need until dma class written
            DMASUSP = 1<<12
    };



};

/*=============================================================================
 all functions inline
=============================================================================*/
uint32_t Osc::m_speed = 0;

//some functions need irq disabled, others can use
//sk.unlock/lock directly

//system unlock for register access, w/irq,dma disable
auto Osc::unlock_irq() -> IDSTAT {
    IDSTAT idstat = (IDSTAT)(ir.all_ison() | r.anybit(DMACON, DMASUSP)<<1);
    r.setbit(DMACON, DMASUSP);
    ir.disable_all();
    sk.unlock();
    return idstat;
}
//system lock enable, restore previous irq status
void Osc::lock_irq(IDSTAT idstat){
    sk.lock();
    if((uint8_t)idstat & 2) r.clrbit(DMACON, DMASUSP);
    if((uint8_t)idstat & 1) ir.enable_all();
}

//osccon
void Osc::frcdiv(DIVS e){
    sk.unlock();
    r.val(OSCCON+3, e); //upper byte access (only FRCDIV in upper byte)
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
    m_speed = 0;
    speed();
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

//spllcon
auto Osc::plldiv() -> DIVS {
    return (DIVS)r.val8(SPLLCON+3);
}
auto Osc::pllmul() -> PLLMUL {
    return (PLLMUL)r.val8(SPLLCON+2);
}
bool Osc::pllfrc(){
    return r.anybit(SPLLCON, PLLICLK);
}
void Osc::pllfrc(bool tf){
    r.setbit(SPLLCON, PLLICLK, tf);
}
//assume SPLL wanted as clock source (why else set pll)
//assume frcpll, unless bool=false then POSC is pll source
void Osc::pllset(PLLMUL m, DIVS d, bool frc){
    IDSTAT irstat  = unlock_irq();
    //need to switch from SPLL to something else
    //switch to frc (hardware does nothing if already frc)
    clksrc(FRCDIV);
    //set new pll vals
    r.val(SPLLCON+3, d);
    r.val(SPLLCON+2, m);
    //pll select
    pllfrc(frc);
    //source to SPLL
    clksrc(SPLL);
    lock_irq(irstat);
    m_speed = 0;
    speed();
}

//refo1con
//16bit->15bit- pass in 16bit value, /2
void Osc::refo_div(uint16_t v){
    r.val(REFO1CON, v>>1);
}
void Osc::refo(bool tf){
    r.setbit(REFO1CON, ON);
}
void Osc::refo_idle(bool tf){
    r.setbit(REFO1CON, SIDL);
}
void Osc::refo_out(bool tf){
    r.setbit(REFO1CON, OE);
}
void Osc::refo_sleep(bool tf){
    r.setbit(REFO1CON, RSLP);
}
void Osc::refo_divsw(bool tf){
    r.setbit(REFO1CON, DIVSWEN);
}
bool Osc::refo_divsw(){
    return r.anybit(REFO1CON, DIVSWEN);
}
bool Osc::refo_active(){
    return r.anybit(REFO1CON, ACTIVE);
}



//refo1trim


//clkstat
bool Osc::ready(CLKRDY e){
    return r.anybit(CLKSTAT, e);
}

//osctun

//misc
uint32_t Osc::speed(){
    if(m_speed) return m_speed;     //already have it
    m_speed = m_default_speed;      //assume default if cannot get
    CNOSC s = clksrc();
    switch(s){
        case LPRC:
            m_speed = 31250;
            break;
        case SOSC:
            m_speed = 32768;//assumed
            break;
        case POSC:          //need something to compare to (lposc vs cp0count)
            break;          //return default for now
        case SPLL: {
            if(!pllfrc()) break;            //is posc
            uint8_t m = (uint8_t)pllmul();  //2 3 4 6 8 12 24
            static const uint8_t tl[] = {2, 3, 4, 6, 8, 12, 24};
            m = tl[m];
            uint8_t d = (uint8_t)plldiv();
            if(d == 7) d = 8;               //adjust DIV256
            m_speed = (8000000*m)>>d;
            break;
        }
        case FRCDIV: {
            uint8_t n = (uint8_t)frcdiv();
            if(n == 7) n = 8;               //adjust DIV256
            m_speed = 8000000>>n;
            break;
        }
        default:
            break;
    }
    return m_speed;
}

