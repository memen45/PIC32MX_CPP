#include "Osc.hpp"
#include "Timer1.hpp"
#include "Cp0.hpp"
#include "Dma.hpp"
#include "Wdt.hpp"
#include "Resets.hpp"
#include "Reg.hpp"
#include "Sys.hpp"
#include "Irq.hpp"

enum {
    OSCCON = 0xBF802680,
        CLKLOCK = 7,
        SLPEN = 4,
        CF = 3,
        SOSCEN = 1,
        OSWEN = 0,
    SPLLCON = 0xBF8026A0,
        PLLICLK = 7,
    REFO1CON = 0xBF802720,
        ON = 15,
        SIDL = 13,
        OE = 12,
        RSLP = 11,
        DIVSWEN = 9,
        ACTIVE = 8,
    REFO1TRIM = 0xBF802730,
    CLKSTAT = 0xBF802770,
    OSCTUN = 0xBF802880,
        /* ON = 15, SIDL = 13, from refo1con*/
        SRC = 12,
        LOCK = 11,
        POL = 10,
        ORNG = 9,
        ORPOL = 8
};

//class vars
//=============================================================================
uint32_t        Osc::m_sysclk = 0;
uint32_t        Osc::m_refoclk = 0;
uint32_t        Osc::m_extclk = 0;
uint32_t        Osc::m_refo_freq = 0;
const uint8_t   Osc::m_mul_lookup[] = {2, 3, 4, 6, 8, 12, 24};
//=============================================================================

//some functions need irq disabled, use-
//  uint8_t v = Sys::unlock_wait(); ... ; Sys::lock(v);
//else use Sys::unlock() and Sys::lock()


//osccon
//=============================================================================
        auto Osc::
    frc_div (DIVS e) -> void
//=============================================================================
{
    Sys::unlock();
    Reg::val(OSCCON + 3, e);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    frc_div () -> DIVS
//=============================================================================
{
    return (DIVS)Reg::val8(OSCCON + 3);
}

//=============================================================================
        auto Osc::
    clk_src () -> CNOSC
//=============================================================================
{
    return (CNOSC)(Reg::val8(OSCCON + 1)>>4);
}

//=============================================================================
        auto Osc::
    clk_src (CNOSC e) -> void
//=============================================================================
{
    uint8_t irstat = Sys::unlock_wait();
    Reg::val(OSCCON + 1, e);
    Reg::setbit(OSCCON, 1<<OSWEN);
    while(Reg::anybit(OSCCON, 1<<OSWEN));
    Sys::lock(irstat);
    m_sysclk = 0;
    sysclk();
}

//=============================================================================
        auto Osc::
    clk_lock () -> void
//=============================================================================
{
    Reg::setbit(OSCCON, 1<<CLKLOCK);
}

//=============================================================================
        auto Osc::
    sleep () -> void
//=============================================================================
{
    //sleep bit only enabled here, then disabled when wakes
    Sys::unlock();
    Reg::setbit(OSCCON, 1<<SLPEN);
    Sys::lock();
    Wdt::reset();
    __asm__ __volatile__ ("wait");
    Sys::unlock();
    Reg::clrbit(OSCCON, 1<<SLPEN);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    sleep_reten () -> void
//=============================================================================
{
    //reten bit only enabled here, then disabled when wakes
    Resets::reten(true);
    sleep();
    Resets::reten(false);
}

//=============================================================================
        auto Osc::
    idle () -> void
//=============================================================================
{
    Wdt::reset();
    __asm__ __volatile__ ("wait");
}

//=============================================================================
        auto Osc::
    clk_bad () -> bool
//=============================================================================
{
    return Reg::anybit(OSCCON, 1<<CF);
}

//=============================================================================
        auto Osc::
    sosc (bool tf) -> void
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(OSCCON, 1<<SOSCEN, tf);
    Sys::lock();
    while(tf and not ready(SOSCRDY));
}

//=============================================================================
        auto Osc::
    sosc () -> bool
//=============================================================================
{
    return Reg::anybit(OSCCON, 1<<SOSCEN);
}

//spllcon
//=============================================================================
        auto Osc::
    pll_div () -> DIVS
//=============================================================================
{
    return (DIVS)Reg::val8(SPLLCON + 3);
}

//=============================================================================
        auto Osc::
    pll_mul () -> PLLMUL
//=============================================================================
{
    return (PLLMUL)Reg::val8(SPLLCON + 2);
}

//=============================================================================
        auto Osc::
    pll_src () -> PLLSRC
//=============================================================================
{
    return (PLLSRC)Reg::anybit(SPLLCON, 1<<PLLICLK);
}

//private, use pll_set to change src
//=============================================================================
        auto Osc::
    pll_src (PLLSRC e) -> void
//=============================================================================
{
    Reg::setbit(SPLLCON, 1<<PLLICLK, e);
    m_refoclk = 0;  //recalculate refo clock
    refoclk();     //as input now may be different
}

//set SPLL as clock source with specified mul/div
//PLLSRC default is FRC
//=============================================================================
        auto Osc::
    pll_set (PLLMUL m, DIVS d, PLLSRC frc) -> void
//=============================================================================
{
    //IDSTAT irstat  = unlock_irq();
    uint8_t irstat  = Sys::unlock_wait();
    //need to switch from SPLL to something else
    //switch to frc (hardware does nothing if already frc)
    clk_src(FRCDIV);
    //set new pll vals
    Reg::val(SPLLCON + 3, d);
    Reg::val(SPLLCON + 2, m);
    //pll select
    pll_src(frc); //do after m, so refoclk() sees new m value
    //source to SPLL
    clk_src(SPLL);
    //lock_irq(irstat);
    Sys::lock(irstat);
}

//refo1con, refo1trim
//=============================================================================
        auto Osc::
    refo_div (uint16_t v) -> void
//=============================================================================
{
    Reg::val(REFO1CON, v);
}

//=============================================================================
        auto Osc::
    refo_trim (uint16_t v) -> void
//=============================================================================
{
    Reg::val(REFO1TRIM + 2, v<<7);
}

//=============================================================================
        auto Osc::
    refo_on () -> void
//=============================================================================
{
    refoclk(); //calculate if needed
    Reg::setbit(REFO1CON, 1<<ON);
    while(refo_active() == 0);
}

//=============================================================================
        auto Osc::
    refo_on (ROSEL e) -> void
//=============================================================================
{
    refo_src(e);
    refo_on();
}

//=============================================================================
        auto Osc::
    refo_off () -> void
//=============================================================================
{
    Reg::clrbit(REFO1CON, 1<<ON);
    while(refo_active());
}

//=============================================================================
        auto Osc::
    refo_idle (bool tf) -> void
//=============================================================================
{
    Reg::setbit(REFO1CON, 1<<SIDL, tf);
}

//=============================================================================
        auto Osc::
    refo_out (bool tf) -> void
//=============================================================================
{
    Reg::setbit(REFO1CON, 1<<OE, tf);
}

//=============================================================================
        auto Osc::
    refo_sleep (bool tf) -> void
//=============================================================================
{
    Reg::setbit(REFO1CON, 1<<RSLP, tf);
}

//=============================================================================
        auto Osc::
    refo_divsw () -> void
//=============================================================================
{
    Reg::setbit(REFO1CON, 1<<DIVSWEN);
    while(Reg::anybit(REFO1CON, 1<<DIVSWEN));
}

//=============================================================================
        auto Osc::
    refo_active () -> bool
//=============================================================================
{
    return Reg::anybit(REFO1CON, 1<<ACTIVE);
}

//anytime source set, get new m_refoclk
//force recalculate by setting m_refoclk to 0
//=============================================================================
        auto Osc::
    refo_src (ROSEL e) -> void
//=============================================================================
{
    bool ison = Reg::anybit(REFO1CON, 1<<ON);
    refo_off();
    if(e == RSOSC) sosc(true);
    Reg::val(REFO1CON, e);
    if(ison) refo_on();
    m_refoclk = 0;
    refoclk();
}

//called by refo_src(), refo_on(), refo_freq()
//=============================================================================
        auto Osc::
    refoclk () -> uint32_t
//=============================================================================
{
    if(m_refoclk) return m_refoclk; //previously calculated
    switch(Reg::val8(REFO1CON)){
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
//=============================================================================
        auto Osc::
    refo_freq (uint32_t v) -> void
//=============================================================================
{
    uint32_t m, n;
    m_refo_freq = 0; //prevent call back to here from refoclk()
    refoclk(); //if not calculated already
    m = (m_refoclk<<8) / v;
    n =  m>>9;
    m and_eq 0x1ff;
    refo_div(n);
    refo_trim(m);
    refo_divsw();
    m_refo_freq = v;
}

//=============================================================================
        auto Osc::
    refo_freq () -> uint32_t
//=============================================================================
{
    return m_refo_freq;
}

//clkstat
//=============================================================================
        auto Osc::
    ready (CLKRDY e) -> bool
//=============================================================================
{
    return Reg::anybit(CLKSTAT, e);
}

//osctun
//=============================================================================
        auto Osc::
    tun_auto (bool tf) -> void
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(OSCTUN, 1<<ON, tf);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    tun_idle (bool tf) -> void
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(OSCTUN, 1<<SIDL, tf);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    tun_src (TUNSRC e) -> void
//=============================================================================
{
    if(e == TSOSC) sosc(true);
    Sys::unlock();
    Reg::setbit(OSCTUN, 1<<SRC, e);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    tun_lock () -> bool
//=============================================================================
{
    return Reg::anybit(OSCTUN, 1<<LOCK);
}

//=============================================================================
        auto Osc::
    tun_lpol (bool tf) -> void
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(OSCTUN, 1<<POL, not tf);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    tun_rng () -> bool
//=============================================================================
{
    return Reg::anybit(OSCTUN, 1<<ORNG);
}

//=============================================================================
        auto Osc::
    tun_rpol (bool tf) -> void
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(OSCTUN, 1<<ORPOL, not tf);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    tun_val (int8_t v ) -> void
//=============================================================================
{
    //linit -32 to +31
    if(v > 31) v = 31;
    if(v < -32) v = -32;
    Sys::unlock();
    Reg::val(OSCTUN, v);
    Sys::lock();
}

//=============================================================================
        auto Osc::
    tun_val () -> int8_t
//=============================================================================
{
    int8_t v = Reg::val8(OSCTUN);
    if(v > 31) v or_eq 0xc0; //is negative, sign extend to 8bits
    return v;
}

//misc
//=============================================================================
        auto Osc::
    sysclk () -> uint32_t
//=============================================================================
{
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
            m_sysclk = (f * m)>>d;
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
//=============================================================================
        auto Osc::
    vcoclk () -> uint32_t
//=============================================================================
{
    uint32_t f = 0;
    f = pll_src() == FRC ? m_frcosc_freq : extclk() ;
    return m_mul_lookup[ pll_mul() ] * f;
}

//get ext clock freq, using sosc if available, or lprc
//and cp0 counter to calculate
//OR if user defined m_extosc_freq, return that
//irq's disabled if calculation needed (re-using unlock_irq)
//will only run once- will assume an ext clock will not change
//=============================================================================
        auto Osc::
    extclk () -> uint32_t
//=============================================================================
{
    if(m_extosc_freq) return m_extosc_freq;
    if(m_extclk) return m_extclk;
    Timer1 t1; Cp0 cp0;
    //IDSTAT irstat  = unlock_irq();
    uint8_t irstat  = Sys::unlock_wait();
    //backup timer1 (we want it, but will give it back)
    uint16_t t1conbak = t1.t1con();
    t1.t1con(0); //stop
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
    t1.t1con(t1conbak);
    //lock_irq(irstat);
    Sys::lock(irstat);
    return m_extclk;
}

//=============================================================================
        auto Osc::
    frcclk () -> uint32_t
//=============================================================================
{
    return m_frcosc_freq;
}
