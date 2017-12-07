/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 testing - using C++

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/



/*=============================================================================
 Includes
=============================================================================*/
#include <cstdint>
#include "Delay.hpp"
#include "Pins.hpp"
#include "Irq.hpp"
#include "Cp0.hpp"
#include "Timer1.hpp"
#include "Timer23.hpp"
#include "Pmd.hpp"
#include "Wdt.hpp"
#include "Rtcc.hpp"


/*=============================================================================
 LED's
=============================================================================*/
Pins leds[] = {                 //group leds
    { Pins::D, 1 },             //RED
    { Pins::C, 3 },             //GREEN
    { Pins::C, 15 },            //BLUE
    { Pins::D, 3 },             //LED1 (invert in timer1/timer2/timer3 irq)
    { Pins::C, 13 }             //LED2 (cp0 irq blinks)
};
Pins& led1 = leds[3];           //reference to specific leds
Pins& led2 = leds[4];

/*=============================================================================
 Switches
=============================================================================*/
Pins sw[] = {                   //true=lowison
    { Pins::B, 9,  true },      //SW1 (rotate delays))
    { Pins::C, 10, true },      //SW2 cp0 irq blink rate++
    { Pins::C, 4,  true }       //SW3 cp0 irq blink rate--
};
Pins& sw1 = sw[0];              //sw1/2/3 references
Pins& sw2 = sw[1];
Pins& sw3 = sw[2];

/*=============================================================================
 Delays
=============================================================================*/
DelayCP0 sw_dly;                //debounce
DelayCP0 dly[3];                //CP0 led delays
uint32_t t_ms[] = {             //led delay ms
    200, 400, 600,
    1000, 2000, 4000,
    4000, 8000, 1600,
    0
};

/*=============================================================================
 Timers
=============================================================================*/
Timer23 timer2( Timer23::T2 );
Timer23 timer3( Timer23::T3 );

/*=============================================================================
 Irq's
=============================================================================*/
Irq::irq_list_t irqlist[] = {               //list of irq's to init/enable
    //vector#           pri sub enable
    { Irq::TIMER_1,     1,  0,  true },
    { Irq::TIMER_2,     1,  0,  true },
    { Irq::TIMER_3,     1,  0,  true },
    { Irq::CORE_TIMER,  1,  0,  true },
    { Irq::RTCC,        1,  0,  true },
    { Irq::END }                            //need END or else bad happens
};

/*=============================================================================
 Modules disable
=============================================================================*/
Pmd::PMD pmd_list[] = {                      //list of modules to disable
    Pmd::USB,
    Pmd::HLVD, Pmd::CMP1, Pmd::CMP2, Pmd::CMP3,
    Pmd::I2C1, Pmd::I2C2, Pmd::I2C3,
    Pmd::T1, Pmd::T2, Pmd::T3,
    Pmd::END                                //need END or else bad happens
};

/*=============================================================================
 MAIN
=============================================================================*/
int main()
{
    //test rtcc (somewhat)
    Rtcc::clk_sel( Rtcc::LPRC );            //internl lprc
    Rtcc::clk_div( Rtcc::CLK_DIV_LPRC );    //already calculated div for lprc
    Rtcc:alarm_interval( Rtcc::MINUTE1 );   //alarm every minute
    Rtcc::chime_on();                       //repeating alarm
    Rtcc::alarm_on();

    const uint32_t test_time = (1<<8 | 0<<12 | 5<<16 | 4<<20 | 3<<24 | 2<<28);
    Rtcc::time( test_time );                //23:45:01
    if( Rtcc::time() != test_time ){        //did write work?
        while( 1 );                         //lockup if read/write not working
    }                                       //else syskey/wrlock working
    Rtcc::on();                             //turn on, alarm is 00:00:00
                                            //(alarm every 00 seconds match)
                                            //disable/enable core timer irq
                                            //every minute

    Wdt::on();                              //try the watchdog
                                            //RWDTPS = PS8192, so 8ms timeout


    Pmd::off( pmd_list );                   //test Pmd disable (T1/2/3 disabled)
                                            //can verify T1,T2,T3 no longer work
                                            //with below enable commented out

    Pmd::on( pmd_list );                    //test Pmd enable- all back on again


    for( auto& s : sw ){                    //init pins
        s.digital_in();
        s.pullup_on();
    }

    for( auto& l : leds ){                  //init leds
        l.digital_out();
        l.on();                             //show each led
        sw_dly.wait_ms( 1000 );             //wait
        l.off();                            //then off
    }

    Timer1::pre_256(); Timer1::on();        //turn on timer1, prescale 1:256
    timer2.pre_64(); timer2.on();           //turn on timer2, prescale 1:64
    timer3.pre_32(); timer3.on();           //turn on timer3, prescale 1:16
    Cp0::compare_ms( 200 );                 //cp0 compare timeout
                                            //(default sysfreq 24MHz)

    //nested function
    auto rotate_delays = [ & ](){           //init delays or rotate delays
        static uint8_t start = 0;
        for( auto& d : dly ){
            if( ! t_ms[start] ) start = 0;
            d.set_ms( t_ms[start++] );
        }
    };
    rotate_delays();                        //initialize delays

    //nested function
    auto irq_blinkrate = [ & ]( int16_t n ){//sw1/2 adjust cp0 irq rate +/-
        static uint16_t rate = 200;
        rate += n;
        if( rate < 100 || rate > 4000 ) rate -= n;
        Cp0::compare_ms( rate );            //no need to disable irq's
        led2.on();                          //as cp0 irq only reloads (reads)
        Irq::enable_all();
    };

    Irq::init( irqlist );                   //init all irq's
    Irq::enable_all();                      //global irq enable

    for (;;){
        Wdt::reset();                       //comment out to test wdt reset

        for( auto i = 0; i < 3; i++ ){
            if( ! dly[i].expired() ) continue;
            leds[i].invert();
            dly[i].reset();
        }
        if( sw1.ison() ){
            if( sw_dly.expired() ){         //is a new press
                rotate_delays();            //do something visible
            }
            sw_dly.set_ms( 100 );           //sw debounce (use for all switches)
            //sw_dly sets while pressed
            //so will require sw release of 100ms
            //before sw_dly timer can expire
        }
        if( sw2.ison() ){
            if( sw_dly.expired() ){
                irq_blinkrate( 100 );       //++
            }
            sw_dly.set_ms( 100 );           //sw debounce (use for all switches)
        }
        if( sw3.ison() ){
            if( sw_dly.expired() ){
                irq_blinkrate( -100 );      //--
            }
            sw_dly.set_ms( 100 );           //sw debounce (use for all switches)
        }
    }
}

/*=============================================================================
 Interrupt code
=============================================================================*/
extern "C" {
    void __attribute__(( vector(0), interrupt(IPL1SOFT) )) CoreTimerISR()
    {
        Cp0::compare_reload();
        led2.invert();
        Irq::flagclear( Irq::CORE_TIMER );
    }
    void __attribute__(( vector(17), interrupt(IPL1SOFT) )) Timer1ISR()
    {
        led1.invert();
        Irq::flagclear( Irq::TIMER_1 );
    }
    void __attribute__(( vector(18), interrupt(IPL1SOFT) )) Timer2ISR()
    {
        led1.invert();
        Irq::flagclear( Irq::TIMER_2 );
    }
    void __attribute__(( vector(19), interrupt(IPL1SOFT) )) Timer3ISR()
    {
        led1.invert();
        Irq::flagclear( Irq::TIMER_3 );
    }
    void __attribute__(( vector(32), interrupt(IPL1SOFT) )) RtccISR()
    {
        static bool b = false;
        b = !b;
        if( b ){
            Irq::disable( Irq::CORE_TIMER );
            led2.off();
        } else {
            Cp0::compare_reload();
            Irq::enable( Irq::CORE_TIMER );
        }
        Irq::flagclear( Irq::RTCC );
    }
}

