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
    static auto
    frc_div (DIVS) -> void;         //set

    static auto
    frc_div () -> DIVS;             //get

    enum CNOSC : uint8_t { //cosc/nosc in second byte of osccon
         FRCDIV = 0, SPLL, POSC, SOSC = 4, LPRC
    };
    static auto
    clk_src () -> CNOSC;            //get current osc source

    static auto
    clk_src (CNOSC) -> void;        //start switch to nosc

    static auto
    clk_lock () -> void;            //lock nosc/oswen until reset

    static auto
    sleep () -> void;               //sleep, wait

    static auto
    sleep_reten () -> void;         //retention sleep, wait

    static auto
    idle () -> void;                //idle, wait

    static auto
    clk_bad () -> bool;             //clock failed?

    static auto
    sosc (bool) -> void;            //sosc enable

    static auto
    sosc () -> bool;                //sosc enabled?
                                    //if enabled, assume its there
    static auto
    pll_div () -> DIVS;             //get pll divider

    enum PLLMUL : uint8_t {
        MUL2, MUL3, MUL4, MUL6, MUL8, MUL12, MUL24
    };
    static auto
    pll_mul () -> PLLMUL;           //get pll multiplier

    enum PLLSRC : bool { EXT, FRC };
    static auto
    pll_src () -> PLLSRC;           //get pll src

    private:
    static auto
    pll_src (PLLSRC) -> void;       //set pll src

    public:
    static auto
    pll_set (PLLMUL, DIVS, PLLSRC = FRC) -> void; //set pll mul/div, pll src

    //refo1con, refo1trim
    static auto
    refo_div (uint16_t) -> void;    //divisor value

    static auto
    refo_trim (uint16_t) -> void;   //trim value

    static auto
    refo_on () -> void;             //refo on

    enum ROSEL : uint8_t {
        RSYSCLK = 0, RPOSC = 2, RFRC, RLPRC, RSOSC, RPLLVCO = 7
    };
    static auto
    refo_on (ROSEL) -> void;        //refo on, src sel

    static auto
    refo_off () -> void;            //refo off

    static auto
    refo_idle (bool) -> void;       //true = stop in idle mode

    static auto
    refo_out (bool) -> void;        //true = clk out to REFO1 pin

    static auto
    refo_sleep (bool) -> void;      //true = run in sleep

    static auto
    refo_divsw () -> void;          //do divider switch

    static auto
    refo_active () -> bool;         //true = active

    static auto
    refo_src (ROSEL) -> void;       //clk source select

    static auto
    refoclk () -> uint32_t;         //get refo in clk freq

    static auto
    refo_freq (uint32_t) -> void;   //set refo frequency

    static auto
    refo_freq () -> uint32_t;       //get refo frequency

    //clkstat
    enum CLKRDY : uint8_t {
        FRCRDY = 1<<0, SPLLDIVRDY = 1<<1, POSCRDY = 1<<2,
        SOSCRDY = 1<<4, LPRCRDY = 1<<5, USBRDY = 1<<6, SPLLRDY = 1<<7
    };
    static auto
    ready (CLKRDY) -> bool;         //clock ready?

    //osctun
    static auto
    tun_auto (bool) -> void;        //osc tune on

    static auto
    tun_idle (bool) -> void;        //stop in idle

    enum TUNSRC : bool { TSOSC, TUSB };
    static auto
    tun_src (TUNSRC) -> void;       //src, 0=sosc 1=usb

    static auto
    tun_lock () -> bool;            //lock status

    static auto
    tun_lpol (bool) -> void;        //irq polarity, 1=0 0=1

    static auto
    tun_rng () -> bool;             //out of range status

    static auto
    tun_rpol (bool) -> void;        //range polarity, 1=0 0=1

    static auto
    tun_val (int8_t) -> void;       //tune value 6bits, -32 to 31

    static auto
    tun_val () -> int8_t;           //get tune value

    //misc
    static auto
    sysclk () -> uint32_t;          //get cpu sysclk

    static auto
    vcoclk () -> uint32_t;          //get pll vco clock

    static auto
    extclk () -> uint32_t;          //get ext clock

    static auto
    frcclk () -> uint32_t;          //get frc clock


    private:

    static uint32_t m_sysclk;       //store calculated cpu freq
    static uint32_t m_refoclk;      //store calculated refo in clk
    static uint32_t m_extclk;
    static uint32_t m_refo_freq;    //store current refo frequency
    static const uint32_t m_frcosc_freq = 8000000;
    static const uint32_t m_extosc_freq = 0; //set if want exact ext freq
    static const uint8_t m_mul_lookup[7];

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
