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
    OSCCON = 0xBF80F000,
        CLKLOCK = 7,
        SLPEN = 4,
        CF = 3,
        SOSCEN = 1,
        OSWEN = 0,
    OSCTUN = 0xBF80F010,
	DEVCFG2 = 0xBFC02FF4
};

uint32_t Osc::m_sysclk = 0;
uint32_t Osc::m_extclk = 0;
const uint8_t Osc::m_mul_lookup[] = {15, 16, 17, 18, 19, 20, 21, 24};
const uint8_t Osc::m_idiv_lookup[] = {1, 2, 3, 4, 5, 6, 10, 12};

//some functions need irq disabled, use-
//  uint8_t v = Sys::unlock_wait(); ... ; Sys::lock(v);
//else use Sys::unlock() and Sys::lock()


//osccon
//=============================================================================
    void            Osc::frc_div        (DIVS e)
//=============================================================================
{
    Sys::unlock();
    Reg::val(OSCCON + 3, e);
    Sys::lock();
}

//=============================================================================
    auto            Osc::frc_div        () -> DIVS
//=============================================================================
{
    return (DIVS)Reg::val8(OSCCON + 3);
}

//=============================================================================
    auto            Osc::clk_src        () -> CNOSC
//=============================================================================
{
    return (CNOSC)(Reg::val8(OSCCON + 1)>>4);
}

//=============================================================================
    void            Osc::clk_src        (CNOSC e)
//=============================================================================
{
    //IDSTAT irstat = unlock_irq();
    uint8_t irstat = Sys::unlock_wait();
    Reg::val(OSCCON + 1, e);
    Reg::setbit(OSCCON, 1<<OSWEN);
    while(Reg::anybit(OSCCON, 1<<OSWEN));
    //lock_irq(irstat);
    Sys::lock(irstat);
    m_sysclk = 0;
    sysclk();
}

//=============================================================================
    void            Osc::clk_lock       ()
//=============================================================================
{
    Reg::setbit(OSCCON, 1<<CLKLOCK);
}

//=============================================================================
    void            Osc::sleep          ()
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
    void            Osc::idle           ()
//=============================================================================
{
    Wdt::reset();
    __asm__ __volatile__ ("wait");
}

//=============================================================================
    bool            Osc::clk_bad        ()
//=============================================================================
{
    return Reg::anybit(OSCCON, 1<<CF);
}

//=============================================================================
    void            Osc::sosc           (bool tf)
//=============================================================================
{
    Sys::unlock();
    Reg::setbit(OSCCON, 1<<SOSCEN, tf);
    Sys::lock();
    while(tf and not ready(SOSCRDY));
}

//=============================================================================
    bool            Osc::sosc           ()
//=============================================================================
{
    return Reg::anybit(OSCCON, 1<<SOSCEN);
}

//spllcon
//=============================================================================
    auto            Osc::pll_odiv        () -> DIVS
//=============================================================================
{
    return (DIVS)((Reg::val8(OSCCON + 3) >> 3) bitand 7);
}

//=============================================================================
    auto            Osc::pll_idiv        () -> IDIVS
//=============================================================================
{
    return (IDIVS)Reg::val8(DEVCFG2);
}

//=============================================================================
    auto            Osc::pll_mul        () -> PLLMUL
//=============================================================================
{
    return (PLLMUL)(Reg::val8(OSCCON + 2) bitand 7);
}

//set SPLL as clock source with specified mul/div
//PLLSRC default is FRC
//=============================================================================
    void            Osc::pll_set        (PLLMUL m, DIVS d, CNOSC frc)
//=============================================================================
{
    //IDSTAT irstat  = unlock_irq();
    uint8_t irstat  = Sys::unlock_wait();
    //need to switch from SPLL to something else
    //switch to frc (hardware does nothing if already frc)
    clk_src(FRCDIV);
    //set new pll vals
    Reg::val(OSCCON + 3, d<<3);
    Reg::val(OSCCON + 2, m);
    //pll select
    clk_src(frc);
    //lock_irq(irstat);
    Sys::lock(irstat);
}

//clkstat
//=============================================================================
    bool            Osc::ready          (CLKRDY e)
//=============================================================================
{
    return Reg::anybit(OSCCON + 2, e);
}

//osctun
//=============================================================================
    void            Osc::tun_val        (int8_t v )
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
    int8_t          Osc::tun_val        ()
//=============================================================================
{
    int8_t v = Reg::val8(OSCTUN);
    if(v > 31) v or_eq 0xc0; //is negative, sign extend to 8bits
    return v;
}

//misc
//=============================================================================
    uint32_t        Osc::sysclk         ()
//=============================================================================
{
    if(m_sysclk) return m_sysclk;           //already have it
    CNOSC s = clk_src();
    switch(s){
        case LPRC: m_sysclk = 31250; break; //+/-15% = 26562,5 - 35937,5
        case SOSC: m_sysclk = 32768; break;
        case POSC: m_sysclk = extclk(); break;
		case FRCPLL:
		case POSCPLL: {
			uint32_t f = s ==FRCPLL ? m_frcosc_freq : extclk();			
			uint8_t m = (uint8_t)pll_mul(); 		//15 16 17 18 19 20 21 24
			m = m_mul_lookup[m];
			uint8_t idiv = (uint8_t) pll_idiv();
			idiv = m_idiv_lookup[idiv];
			uint8_t odiv = (uint8_t)pll_odiv();
			if (odiv == 7) odiv = 8;               //adjust DIV256
			m_sysclk = ((f / idiv) * m) >> idiv;
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

//get ext clock freq, using sosc if available, or lprc
//and cp0 counter to calculate
//OR if user defined m_extosc_freq, return that
//irq's disabled if calculation needed (re-using unlock_irq)
//will only run once- will assume an ext clock will not change
//=============================================================================
    uint32_t        Osc::extclk         ()
//=============================================================================
{
    if(m_extosc_freq) return m_extosc_freq;
    if(m_extclk) return m_extclk;
    Timer1 t1; Cp0 cp0;
    //IDSTAT irstat  = unlock_irq();
    uint8_t irstat  = Sys::unlock_wait();
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
    //lock_irq(irstat);
    Sys::lock(irstat);
    return m_extclk;
}

//=============================================================================
    uint32_t        Osc::frcclk         ()
//=============================================================================
{
    return m_frcosc_freq;
}
