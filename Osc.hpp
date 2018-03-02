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

 any peripheral specifying SOSC as clock source, will also use
 Osc::sosc(true) to make sure sosc is turned on, but no check if SOSC is
 present (assume if specified, caller must know its available), if peripheral
 like rtcc needs a clock but is not specified, then peripheral will call
 sosc() to check if its on already, if on assume its available

 if using sosc, just call Osc::sosc(true) early in code (may already be fused
 on), so any peripheral needing it can use it
*/

#include <cstdint>

struct Osc {

    //osccon
    enum DIVS : uint8_t { //upper byte of osccon, spllcon
        DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV256
    };
    static void         frc_div     (DIVS);     //set
    static DIVS         frc_div     ();         //get

    enum CNOSC : uint8_t { //cosc/nosc in second byte of osccon
         //FRCDIV = 0, SPLL, POSC, SOSC = 4, LPRC
		 FRC = 0, FRCPLL, POSC, POSCPLL, SOSC, LPRC, FRC16, FRCDIV
    };
    static CNOSC        clk_src     ();         //get current osc source

    static void         clk_src     (CNOSC);    //start switch to nosc
    
    static void         clk_lock    ();         //lock nosc/oswen until reset
    static void         sleep       ();         //sleep, wait
    static void         sleep_reten ();         //retention sleep, wait
    static void         idle        ();         //idle, wait
    static bool         clk_bad     ();         //clock failed?
    static void         sosc        (bool);     //sosc enable
    static bool         sosc        ();         //sosc enabled?
                                                //if enabled, assume its there
    
    static DIVS         pb_div      ();         //get current Peripheral Bus clock divider
    static void         pb_div      (DIVS);     //set current peripheral bus clock divider
    
    //spllcon
    enum IDIVS : uint8_t { //lower byte of devcfg2
        IDIV1, IDIV2, IDIV3, IDIV4, IDIV5, IDIV6, IDIV10, IDIV12
    };
    static DIVS         pll_odiv    ();         //get pll divider
	static IDIVS		pll_idiv	();			//get pll input divider

    enum PLLMUL : uint8_t {
        MUL15 = 0, MUL16, MUL17, MUL18, MUL19, MUL20, MUL21, MUL24
    };
    static PLLMUL       pll_mul     ();         //get pll multiplier

    public:
    static void         pll_set     (PLLMUL, DIVS, CNOSC = FRC);
                                                //set pll mul/div, pll src

    //clkstat
    enum CLKRDY : uint32_t {
//        FRCRDY = 1<<0, SPLLDIVRDY = 1<<1, POSCRDY = 1<<2,
//        SOSCRDY = 1<<4, LPRCRDY = 1<<5, USBRDY = 1<<6, SPLLRDY = 1<<7
		SOSCRDY = 1<<6, PBDIVRDY = 1<<5
    };
    static bool         ready       (CLKRDY);   //clock ready?

    //osctun

    static void         tun_val     (int8_t);   //tune value 6bits, -32 to 31
    static int8_t       tun_val     ();         //get tune value

    //misc
    static uint32_t     sysclk       ();        //get cpu sysclk
    static uint32_t     extclk       ();        //get ext clock
    static uint32_t     frcclk       ();        //get frc clock
    static uint32_t     pbclk        ();

    private:

    static uint32_t m_sysclk;                    //store calculated cpu freq
    static uint32_t m_extclk;
    static uint32_t m_pbclk;

    static const uint32_t m_frcosc_freq = 8000000;
    static const uint32_t m_extosc_freq = 0;    //set if want exact ext freq
    static const uint8_t m_mul_lookup[8];
	static const uint8_t m_idiv_lookup[8];

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
