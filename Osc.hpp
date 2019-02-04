#pragma once

//Oscillator

/*
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

 any peripheral specifying SOSC as clock source, will also cause use of
 Osc::sosc(true) to make sure sosc is turned on, but no check if SOSC is
 present (assume if specified, caller must know its available), if peripheral
 like rtcc needs a clock but is not specified, then peripheral code will call
 sosc() to check if its on already, if on assume its available

 if using sosc, just call Osc::sosc(true) early in code (may already be fused
 on), so any peripheral needing it can use it
*/

#include <cstdint>
#include "Reg.hpp"

struct Osc : private Reg {

            //==== OSCCON ====

            enum
DIVS        : uint8_t { //upper byte of osccon, spllcon
            DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV256
            };

            static auto
frc_div     (DIVS) -> void;

            static auto
frc_div     () -> DIVS;

            enum
CNOSC       : uint8_t { //cosc/nosc in second byte of osccon
            //FRCDIV = 0, SPLL, POSC, SOSC = 4, LPRC
            FRC = 0, FRCPLL, POSC, POSCPLL, SOSC, LPRC, FRC16, FRCDIV
            };

            //get current osc source
            static auto
clk_src     () -> CNOSC;

            //start switch to nosc
            static auto
clk_src     (CNOSC) -> void;

            //lock nosc/oswen until reset
            static auto
clk_lock    () -> void;

            //sleep, wait
            static auto
sleep       () -> void;

            //retention sleep, wait
            static auto
sleep_reten () -> void;

            //idle, wait
            static auto
idle        () -> void;

            //clock failed?
            static auto
clk_bad     () -> bool;

            //sosc enable
            static auto
sosc        (bool) -> void;

            //sosc enabled? (if enabled, assume its available for use)
            static auto
sosc        () -> bool;
            
            // get peripheral bus divider
            static auto
pb_div      () -> DIVS;
            
            // set peripheral bus divider
            static auto
pb_div      (DIVS) -> void;
            
            //get pll divider
            static auto
pll_odiv    () -> DIVS;
            
            enum 
IDIVS       : uint8_t { //lower byte of devcfg2
            IDIV1, IDIV2, IDIV3, IDIV4, IDIV5, IDIV6, IDIV10, IDIV12
            };
            //get pll input divider
            static auto 
pll_idiv	() -> IDIVS;			

            enum
PLLMUL      : uint8_t {
            MUL15, MUL16, MUL17, MUL18, MUL19, MUL20, MUL21, MUL24
            };

            //get pll multiplier
            static auto
pll_mul     () -> PLLMUL;

            //set pll mul/div, pll src
            static auto
pll_set     (PLLMUL, DIVS, CNOSC = FRC) -> void;
            
            //==== CLKSTAT ====

            enum
CLKRDY      : uint32_t {
            SOSCRDY = 1<<6, PBDIVRDY = 1<<5
            };

            //clock ready?
            static auto
ready       (CLKRDY) -> bool;


            //==== OSCTUN ====

            //tune value 6bits, -32 to 31
            static auto
tun_val     (int8_t) -> void;

            //get tune value
            static auto
tun_val     () -> int8_t;


            //misc

            //get cpu sysclk
            static auto
sysclk      () -> uint32_t;

            //get ext clock
            static auto
extclk      () -> uint32_t;

            //get frc clock
            static auto
frcclk      () -> uint32_t;            
            
            //get peripheral clock
            static auto
pbclk      () -> uint32_t;


            private:

            //store calculated cpu freq
            static uint32_t         m_sysclk;
            //store calculated refo in clk
            static uint32_t         m_extclk;
            //store calculated pbclock in clk
            static uint32_t m_pbclk;
            //store current refo frequency
            static const uint32_t   m_frcosc_freq = 8000000;
            //manually set here if want exact ext freq
            static const uint32_t   m_extosc_freq = 8000000;
            static const uint8_t    m_mul_lookup[8];
            static const uint8_t    m_idiv_lookup[8];
};


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
