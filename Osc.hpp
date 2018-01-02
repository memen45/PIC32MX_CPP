#pragma once

/*=============================================================================
 Oscillator - set cpu clock source, speed

 any fucntion that depends on cpu frequency, can call speed() to get current
 cpu frequency in Hz, if not already calculated, it will be calculated when
 called or when cpu source/speed is changed, if unable to calculate (if ext
 crystal at the moment) then the default frequency is returned

 config bits will need to enable clock source switching (if needed)
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"
#include "Irq.hpp"

struct Osc {

    //public

    //osccon
    enum DIVS : uint8_t { //upper byte of osccon, spllcon
        DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV256
    };
    enum CNOSC : uint8_t { //cosc/nosc in second byte of osccon
         FRC = 0, SPLL, POSC, SOSC = 4, LPRC
    };

    static void         frcdiv      (DIVS);     //set
    static DIVS         frcdiv      ();         //get
    static CNOSC        source      ();         //get current osc source
    static void         source      (CNOSC);    //start switch to nosc
    static bool         locked      ();         //get clock locked status
    static void         sleep       (bool);     //sleep enable
    static bool         failed      ();         //clock failed?
    static void         sosc        (bool);     //sosc enable


    //spllcon
    enum PLLMUL : uint8_t {
        MUL2 = 0, MUL3, MUL4, MUL6, MUL8, MUL12, MUL24
    };

    static DIVS         plldiv      ();         //get pll divider
    static PLLMUL       pllmul      ();         //get pll multiplier
    static bool         pllfrc      ();         //true=pll src frc, false=posc
    static void         pllset      (PLLMUL, DIVS); //set pll mul/div


    //refo1con


    //refo1trim


    //clkstat

    enum CLKRDY : uint8_t {
        FRCRDY = 0, SPLLDIVRDY, POSCRDY,
        SOSCRDY = 4, LPRCRDY, USBRDY, SPLLRDY
    };

    static bool         ready       (CLKRDY);   //clock ready?


    //osctun


    static uint32_t     speed       ();         //get cpu speed (0=unknown)


    private:

    static Reg r;                               //for static class access
    static Syskey sk;                           //I like better than ::
    static Irq ir;

    static uint32_t m_speed;                    //store calculated cpu freq
                                                //returned from speed() if
                                                //not 0, if 0 then calculate
                                                //again (any change in clock
                                                //will run speed() )

    static const uint32_t m_default_speed = 24000000;
//    static const uint8_t m_mul_lookup[];

    enum {
        OSCCON = 0xBF802680,
            COSC_SHIFT = 4, //byte accessed via osccon+1
            CLKLOCK = 1<<7, SLPEN = 1<<5, CF = 1<<3,
            SOSCEN = 1<<1, OSWEN = 1<<0,
        SPLLCON = 0xBF8026A0,
            PLLICLK = 1<<7,
        REFO1CON = 0xBF802720,
        REFO1TRIM = 0xBF802730,
        CLKSTAT = 0xBF802770,
        OSCTUN = 0xBF802880
    };



};

/*=============================================================================
 all functions inline
=============================================================================*/
uint32_t Osc::m_speed = 0;
//const uint8_t Osc::m_mul_lookup[] = {2, 3, 4, 6, 8, 12, 24};

void Osc::frcdiv(DIVS e){
    sk.unlock();
    r.val(OSCCON+3, e); //upper byte access (only FRCDIV in upper byte)
    sk.lock();
}
Osc::DIVS Osc::frcdiv(){
    return (DIVS)r.val8(OSCCON+3);
}
Osc::CNOSC Osc::source(){
    return (CNOSC)(r.val8(OSCCON+1)>>4);
}
void Osc::source(CNOSC e){
    bool irstatus = ir.all_ison();
    ir.disable_all();
    sk.unlock();
    r.val(OSCCON+1, e); //can't over-write cosc, so write whole byte
    r.setbit(OSCCON, OSWEN);
    while(r.anybit(OSCCON, OSWEN));
    sk.lock();
    m_speed = 0; //make speed() actually check it again
    speed();
    if(irstatus) ir.enable_all();
}
bool Osc::locked(){
    return r.anybit(OSCCON, CLKLOCK);
}
void Osc::sleep(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SLPEN, tf);
    sk.lock();
}
bool Osc::failed(){
    return r.anybit(OSCCON, CF);
}
void Osc::sosc(bool tf){
    sk.unlock();
    r.setbit(OSCCON, SOSCEN, tf);
    sk.lock();
}
Osc::DIVS Osc::plldiv(){
    return (DIVS)r.val8(SPLLCON+3);
}
Osc::PLLMUL Osc::pllmul(){
    return (PLLMUL)r.val8(SPLLCON+2);
}
bool Osc::pllfrc(){
    return r.anybit(SPLLCON, PLLICLK);
}
void Osc::pllset(PLLMUL m, DIVS d){
    if(source() != SPLL) return; //pll not in use
    bool irstatus = ir.all_ison();
    ir.disable_all();
    sk.unlock();
    //need to switch from SPLL to something else
    //or 12x MUL will be half of expected
    source(FRC);
    r.val(SPLLCON+3, d);
    r.val(SPLLCON+2, m);
    while(!ready(SPLLRDY));
    //back to SPLL
    source(SPLL);
    sk.lock();
    if(irstatus) ir.enable_all();
    m_speed = 0;
    speed();
}

uint32_t Osc::speed(){
    if(m_speed) return m_speed;     //already have it
    CNOSC s = source();
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
        case FRC: {
            uint8_t n = (uint8_t)frcdiv();
            if(n == 7) n = 8;               //adjust DIV256
            m_speed = 8000000>>n;
            break;
        }
        default:
            break;
    }
    if(!m_speed) m_speed = m_default_speed;
    return m_speed;
}

bool Osc::ready(CLKRDY e){
    return r.anybit(CLKSTAT, e);
}