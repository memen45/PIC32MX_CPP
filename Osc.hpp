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
         FRCDIV = 0, SPLL, POSC, SOSC = 4, LPRC
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

    //spllcon
    //enum DIVS : uint8_t { //upper byte of osccon, spllcon
    //    DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV256
    //}; duplicate
    static DIVS         pll_div     ();         //get pll divider

    enum PLLMUL : uint8_t {
        MUL2 = 0, MUL3, MUL4, MUL6, MUL8, MUL12, MUL24
    };
    static PLLMUL       pll_mul     ();         //get pll multiplier

    enum PLLSRC : bool { EXT, FRC };
    static PLLSRC       pll_src     ();         //get pll src

    private:
    static void         pll_src     (PLLSRC);   //set pll src
    public:
    static void         pll_set     (PLLMUL, DIVS, PLLSRC = FRC);
                                                //set pll mul/div, pll src

    //refo1con, refo1trim
    static void         refo_div    (uint16_t); //divisor value
    static void         refo_trim   (uint16_t); //trim value
    static void         refo_on     ();         //refo on

    enum ROSEL : uint8_t {
        RSYSCLK = 0, RPOSC = 2, RFRC, RLPRC, RSOSC, RPLLVCO = 7
    };
    static void         refo_on     (ROSEL);    //refo on, src sel

    static void         refo_off    ();         //refo off
    static void         refo_idle   (bool);     //true = stop in idle mode
    static void         refo_out    (bool);     //true = clk out to REFO1 pin
    static void         refo_sleep  (bool);     //true = run in sleep
    static void         refo_divsw  ();         //do divider switch
    static bool         refo_active ();         //true = active

    //enum ROSEL : uint8_t {
    //    RSYSCLK = 0, RPOSC = 2, RFRC, RLPRC, RSOSC, RPLLVCO = 7
    //}; duplicate
    static void         refo_src    (ROSEL);    //clk source select
    static uint32_t     refoclk     ();         //get refo in clk freq
    static void         refo_freq   (uint32_t); //set refo frequency
    static uint32_t     refo_freq   ();         //get refo frequency

    //clkstat
    enum CLKRDY : uint8_t {
        FRCRDY = 1<<0, SPLLDIVRDY = 1<<1, POSCRDY = 1<<2,
        SOSCRDY = 1<<4, LPRCRDY = 1<<5, USBRDY = 1<<6, SPLLRDY = 1<<7
    };
    static bool         ready       (CLKRDY);   //clock ready?

    //osctun
    static void         tun_auto    (bool);     //osc tune on
    static void         tun_idle    (bool);     //stop in idle

    enum TUNSRC : bool { TSOSC, TUSB };
    static void         tun_src     (TUNSRC);   //src, 0=sosc 1=usb

    static bool         tun_lock    ();         //lock status
    static void         tun_lpol    (bool);     //irq polarity, 1=0 0=1
    static bool         tun_rng     ();         //out of range status
    static void         tun_rpol    (bool);     //range polarity, 1=0 0=1
    static void         tun_val     (int8_t);   //tune value 6bits, -32 to 31
    static int8_t       tun_val     ();         //get tune value

    //misc
    static uint32_t     sysclk       ();        //get cpu sysclk
    static uint32_t     vcoclk       ();        //get pll vco clock
    static uint32_t     extclk       ();        //get ext clock
    static uint32_t     frcclk       ();        //get frc clock

    private:

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
