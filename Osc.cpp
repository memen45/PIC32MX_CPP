#include "Osc.hpp"
#include "Timer1.hpp"
#include "Cp0.hpp"
#include "Dma.hpp"
#include "Wdt.hpp"
#include "Resets.hpp"
#include "Sys.hpp"
#include "Irq.hpp"

enum {
    OSCCON = 0xBF80F000,
		PLLODIV_SHIFT = 27, PLLODIV_MASK = 7,
        FRCDIV_SHIFT = 24, FRCDIV_MASK = 7,
        PBDIV_SHIFT = 19, PBDIV_MASK = 3,
		PLLMUL_SHIFT = 16, PLLMUL_MASK = 7,
        COSC_SHIFT = 12, COSC_MASK = 7,
        NOSC_SHIFT = 8, NOSC_MASK = 7,
        CLKLOCK = 7,
        SLPEN = 4,
        CF = 3,
        SOSCEN = 1,
        OSWEN = 0,
    OSCTUN = 0xBF80F010,
	DEVCFG2 = 0xBFC02FF4,
		PLLIDIV_SHIFT = 0, PLLIDIV_MASK = 7
};

uint32_t Osc::m_sysclk = 0;
uint32_t Osc::m_extclk = 0;
uint32_t Osc::m_pbclk = 0;
const uint8_t Osc::m_mul_lookup[] = {15, 16, 17, 18, 19, 20, 21, 24};
const uint8_t Osc::m_idiv_lookup[] = {1, 2, 3, 4, 5, 6, 10, 12};

//some functions need irq disabled, use-
//  uint8_t v = Sys::unlock_wait(); ... ; Sys::lock(v);
//else use Sys::unlock() and Sys::lock()

            auto Osc::
frc_div     (DIVS e) -> void
            {
            Sys::unlock();
            val(OSCCON + 3, e);
            Sys::lock();
            }

//=============================================================================
            auto Osc::
frc_div     () -> DIVS
            {
            return (DIVS) (val8(OSCCON + 3) bitand FRCDIV_MASK);
            }

//=============================================================================
            auto Osc::
clk_src     () -> CNOSC
            {
            return (CNOSC)((val(OSCCON) >> COSC_SHIFT) bitand COSC_MASK);
            }

//=============================================================================
            auto Osc::
clk_src     (CNOSC e) -> void
            {
            uint8_t irstat = Sys::unlock_wait();
            clrbit(OSCCON, NOSC_MASK << NOSC_SHIFT);
            setbit(OSCCON, e << NOSC_SHIFT);
            setbit(OSCCON, 1<<OSWEN);
            while(anybit(OSCCON, 1<<OSWEN));
            Sys::lock(irstat);
            m_sysclk = 0;
            m_pbclk = 0;
            sysclk();
            }

//=============================================================================
            auto Osc::
clk_lock    () -> void
            {
            setbit(OSCCON, 1<<CLKLOCK);
            }

//=============================================================================
            auto Osc::
sleep       () -> void
            {
            //sleep bit only enabled here, then disabled when wakes
            Sys::unlock();
            setbit(OSCCON, 1<<SLPEN);
            Sys::lock();
            Wdt::reset();
            __asm__ __volatile__ ("wait");
            Sys::unlock();
            clrbit(OSCCON, 1<<SLPEN);
            Sys::lock();
            }

//=============================================================================
            auto Osc::
idle        () -> void
            {
            Wdt::reset();
            __asm__ __volatile__ ("wait");
            }

//=============================================================================
            auto Osc::
clk_bad     () -> bool
            {
            return anybit(OSCCON, 1<<CF);
            }

//=============================================================================
            auto Osc::
sosc        (bool tf) -> void
            {
            Sys::unlock();
            setbit(OSCCON, 1<<SOSCEN, tf);
            Sys::lock();
            while(tf != ready(SOSCRDY));
            //if turned on, wait for sosc start-up timer to expire
            //but probably no need to wait around
            }

//=============================================================================
            auto Osc::
sosc        () -> bool
            {
            return anybit(OSCCON, 1<<SOSCEN);
            }

//=============================================================================
            auto Osc::
pb_div      (DIVS e) -> void
            {
            Sys::unlock();
            setbit(OSCCON, PBDIV_MASK << PBDIV_SHIFT);
            clrbit(OSCCON, (~(e bitand PBDIV_MASK)) << PBDIV_SHIFT);
            Sys::lock();
            }

//=============================================================================
            auto Osc::
pb_div      () -> DIVS
            {
            return (DIVS) ((val(OSCCON) >> PBDIV_SHIFT) bitand PBDIV_MASK);
            }

//spllcon
//=============================================================================
            auto Osc::
pll_odiv    () -> DIVS
            {
            return (DIVS)((val(OSCCON) >> PLLODIV_SHIFT) bitand PLLODIV_MASK);
            }

//=============================================================================
            auto Osc::
pll_idiv    () -> IDIVS
            {
            return (IDIVS) (val8(DEVCFG2) bitand PLLIDIV_MASK);
            }

//=============================================================================
            auto Osc::
pll_mul     () -> PLLMUL
            {
            return (PLLMUL)(val8(OSCCON + 2) bitand PLLMUL_MASK);
            }

//set SPLL as clock source with specified mul/div
//PLLSRC default is FRC
//=============================================================================
            auto Osc::
pll_set     (PLLMUL m, DIVS d, CNOSC frc) -> void
            {
            //IDSTAT irstat  = unlock_irq();
            uint8_t irstat  = Sys::unlock_wait();
            //need to switch from SPLL to something else
            //switch to frc (hardware does nothing if already frc)
            clk_src(FRCDIV);
            //set new pll vals
            //Reg::val(OSCCON + 3, d<<3); <-- WRITE TO ODD ADDRESS: REMOVE
            clrbit(OSCCON, PLLODIV_MASK << PLLODIV_SHIFT);
            setbit(OSCCON, d << PLLODIV_SHIFT);
            //Reg::val(OSCCON + 2, m); <-- WRITE TO ODD ADDRESS: REMOVE
            clrbit(OSCCON, PLLMUL_MASK << PLLMUL_SHIFT);
            setbit(OSCCON, m << PLLMUL_SHIFT);
            //pll select
            clk_src(frc);
            //lock_irq(irstat);
            Sys::lock(irstat);
            }

//clkstat
//=============================================================================
            auto Osc::
ready       (CLKRDY e) -> bool
            {
            return anybit(OSCCON + 2, e);
            }

//osctun
//=============================================================================
            auto Osc::
tun_val     (int8_t v ) -> void
            {
            //linit -32 to +31
            if(v > 31) v = 31;
            if(v < -32) v = -32;
            Sys::unlock();
            val(OSCTUN, v);
            Sys::lock();
            }

//=============================================================================
            auto Osc::
tun_val     () -> int8_t
            {
            int8_t v = val8(OSCTUN);
            if(v > 31) v or_eq 0xc0; //is negative, sign extend to 8bits
            return v;
            }

//misc
//=============================================================================
            auto Osc::
sysclk      () -> uint32_t
            {
            if(m_sysclk) return m_sysclk;           //already have it 
            CNOSC s = clk_src();       
            switch(s){
                case LPRC: m_sysclk = 31250; break; //+/-15% = 26562,5 - 35937,5
                case SOSC: m_sysclk = 32768; break;
                case POSC: m_sysclk = extclk(); break;
				case FRC: m_sysclk = m_frcosc_freq; break;
				case FRC16: m_sysclk = m_frcosc_freq >> 4;
                case FRCPLL:
                case POSCPLL: {
                    uint32_t f = (s == FRCPLL) ? m_frcosc_freq : extclk();
                    uint8_t m = (uint8_t)pll_mul(); 		//15 16 17 18 19 20 21 24
                    m = m_mul_lookup[m];
                    uint8_t idiv = (uint8_t) pll_idiv();
                    idiv = m_idiv_lookup[idiv];
                    uint8_t odiv = (uint8_t)pll_odiv();
                    if (odiv == 7) odiv = 8;               //adjust DIV256
                    m_sysclk = ((f / idiv) * m) >> odiv;
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

//get ext clock freq, using sosc if available, or lprc
//and cp0 counter to calculate
//OR if user defined m_extosc_freq, return that
//irq's disabled if calculation needed (re-using unlock_irq)
//will only run once- will assume an ext clock will not change
//=============================================================================
            auto Osc::
extclk      () -> uint32_t
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
			if (!sosc()) sosc(true);
			t1.clk_src(t1.T1CK);
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
frcclk      () -> uint32_t
            {
            return m_frcosc_freq;
            }
            
//=============================================================================
            auto Osc::
pbclk       () -> uint32_t
            {
            if (m_pbclk) return m_pbclk;
            m_pbclk = sysclk() >> pb_div();
            return m_pbclk;
            }
            
// auto calculate dividers or pll settings for the desired clk and speed
//=============================================================================
            auto Osc::
clk_switch	(CNOSC e, uint32_t clk) -> void
			{
			uint32_t old = sysclk();
			switch(e){
                case LPRC: 
                case SOSC:  
                case POSC:  
				case FRC16:
                case FRCDIV:
				case FRC: {
					clk_src(e);									// perform the switch
					uint32_t new_clk = sysclk();
					if (old > new_clk)
						Sys::waitstates(new_clk);				// NOTE: clk speed below 30 000 000 - waitstates are 0, so fix after clk_src(e) is fine
					break;
				}
                case FRCPLL:
                case POSCPLL: {
                    uint32_t f = (e == FRCPLL) ? m_frcosc_freq : extclk();
                    uint8_t idiv = (uint8_t) pll_idiv();
                    idiv = m_idiv_lookup[idiv];
					f /= idiv;									// fixed idiv
											
					uint8_t m = 0;										//15 16 17 18 19 20 21 24
                    uint8_t odiv = 0;									// 1  2  4  8 16 32 64 256
					uint32_t calc_clk = f * m_mul_lookup[m];	// fix multiplier
					while (calc_clk > clk)						// odiv should only be increased if larger than desired clock
					{
						calc_clk >>= 1;
						odiv++;
						if (odiv == 7) break;
					}
					if (odiv == 7) odiv = 8;					//adjust DIV256
					
					f >>= odiv;									// fix odiv
					calc_clk = f * m_mul_lookup[m];				
					while (calc_clk < clk)						// mul should only be increased if smaller than desired clock
						calc_clk = f * m_mul_lookup[++m];
					
					// start switching
					if (calc_clk > old)	Sys::waitstates(calc_clk);	// increase clock ? increase waitstates BEFORE switch, otherwise flash read not possible
					pll_set(static_cast<PLLMUL>(m), static_cast<DIVS>(odiv), e);							// perform the switch
					if (calc_clk < old) Sys::waitstates(calc_clk);	// decrease clock ? decrease waitstates for faster flash read
                    break;
                }
                default:
                    break;
            }
			}
