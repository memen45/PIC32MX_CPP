
#include <cstdint>
#include "Delay.hpp"
#include "Pins.hpp"
#include "Irq.hpp"
#include "Cp0.hpp"
#include "Timer1.hpp"
#include "Timer23.hpp"

Pins leds[] = {
    { Pins::PORT::D, 1 },       //RED
    { Pins::PORT::C, 3 },       //GREEN
    { Pins::PORT::C, 15 },      //BLUE
    { Pins::PORT::D, 3 },       //LED1 (invert in timer1/timer2/timer3 irq)
    { Pins::PORT::C, 13 }       //LED2 (cp0 irq blinks)
};
Pins& led1 = leds[3];
Pins& led2 = leds[4];

Pins sw[] = {
    { Pins::PORT::B, 9 },       //SW1 (rotate delays))
    { Pins::PORT::C, 10 },      //SW2 cp0 irq blink rate++
    { Pins::PORT::C, 4 }        //SW3 cp0 irq blink rate--
};
Pins& sw1 = sw[0];
Pins& sw2 = sw[1];
Pins& sw3 = sw[2];

DelayCP0 sw_dly;                //debounce
DelayCP0 dly[3];                //CP0 led delays
uint32_t t_ms[] = {             //led delay ms
    200,4000,4000,
    4000,200,4000,
    4000,4000,200,
    0
};

Timer23 timer2( Timer23::T2 );
Timer23 timer3( Timer23::T3 );

Irq::irq_list_t irqlist[] = {   //list of irq's to init/enable
    //vector#           pri sub enable
    { Irq::TIMER_1,     1,  0,  true },
    { Irq::TIMER_2,     1,  0,  true },
    { Irq::TIMER_3,     1,  0,  true },
    { Irq::CORE_TIMER,  1,  0,  true },
    { Irq::END_LIST }
};

int main()
{
    for( auto& s : sw ){                    //init pins
        s.digital_in();
        s.pullup_on();
        s.lowison();
    }

    for( auto& l : leds ){                  //init leds
        l.digital_out();
        l.on();                             //show each led
        sw_dly.wait_ms( 2000 );             //wait
        l.off();                            //then off
    }

    Timer1::pre_256(); Timer1::on();        //turn on timer1, prescale 1:256
    timer2.pre_64(); timer2.on();           //turn on timer2, prescale 1:64
    timer3.pre_32(); timer3.on();           //turn on timer3, prescale 1:16
    Cp0::compare_ms( 200 );                 //cp0 compare timeout
                                            //(default sysfreq 24MHz)

    //init delays or rotate delays
    auto rotate_delays = [ & ](){
        static uint8_t start = 0;
        for( auto& d : dly ){
            if( ! t_ms[start] ) start = 0;
            d.set_ms( t_ms[start++] );
        }
    };
    rotate_delays();                        //initialize delays

    auto irq_blinkrate = [ & ]( int16_t n ){//sw1/2 adjust cp0 irq rate +/-
        static uint16_t rate = 200;
        rate += n;
        if( rate < 100 || rate > 4000 ) rate -= n;
        Irq::disable_all();
        Cp0::compare_ms( rate );
        led2.on();
        Irq::enable_all();
    };

    Irq::init( irqlist );                   //init all irq's
    Irq::enable_all();                      //global irq enable

    for (;;){
        for( auto i = 0; i < 3; i++ ){
            if( ! dly[i].isexpired() ) continue;
            leds[i].invert();
            dly[i].reset();
        }
        if( sw1.ison() ){
            if( sw_dly.isexpired() ){       //is a new press
                rotate_delays();            //do something visible
            }
            sw_dly.set_ms( 100 );           //sw debounce (use for allswitches)
            //sw_dly sets while pressed
            //so will require sw release of 100ms
            //before sw_dly timer can expire
        }
        if( sw2.ison() ){
            if( sw_dly.isexpired() ){
                irq_blinkrate( 100 );       //++
            }
            sw_dly.set_ms( 100 );           //sw debounce (use for allswitches)
        }
        if( sw3.ison() ){
            if( sw_dly.isexpired() ){
                irq_blinkrate( -100 );      //--
            }
            sw_dly.set_ms( 100 );           //sw debounce (use for allswitches)
        }
    }
}

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
}

