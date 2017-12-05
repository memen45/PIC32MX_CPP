
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
uint32_t t_ms[3] = {            //led delay ms
    200,800,1200
};

Timer23 timer2( Timer23::T2 );
Timer23 timer3( Timer23::T3 );

int main()
{
    for( auto& s : sw ){ s.digital(); s.pullup_on(); s.lowison();}  //init pins
    for( auto& led : leds ){ led.output(); led.digital(); }         //init leds
    Timer1::pre_256(); Timer1::on();        //turn on timer1, prescale 1:256
    timer2.pre_64(); timer2.on();           //turn on timer2, prescale 1:64
    timer3.pre_32(); timer3.on();           //turn on timer3, prescale 1:16
    Cp0::compare_ms( 200 );     //cp0 compare timeout (default sysfreq 24MHz)
    //irq's
    Irq::priority_en( Irq::TIMER_1, Irq::IRQ_PRI::PRI1 );
    Irq::priority_en( Irq::TIMER_2, Irq::IRQ_PRI::PRI1 );
    Irq::priority_en( Irq::TIMER_3, Irq::IRQ_PRI::PRI1 );
    Irq::priority_en( Irq::CORE_TIMER, Irq::IRQ_PRI::PRI1 );
    Irq::enable_all();          //global irq enable

    //init delays or rotate delays
    auto rotate_delays = [ & ](){
        static uint8_t start = 0;
        for( auto& d : dly ){
            if( start >= 3 ) start -= 3;
            d.set_ms( t_ms[start++] );
        }
        start++;
    };
    rotate_delays();            //initialize delays

    auto irq_blinkrate = [ & ]( int16_t n ){
        static uint16_t rate = 200;
        rate += n;
        if( rate < 100 || rate > 4000 ) rate -= n;
        Irq::disable_all();
        Cp0::compare_ms( rate );
        Irq::enable_all();
    };

    for (;;){
        for( auto i = 0; i < 3; i++ ){
            if( ! dly[i].isexpired() ) continue;
            leds[i].invert();
            dly[i].reset();
        }
        if( sw1.ison() ){
            if( sw_dly.isexpired() ){   //is a new press
                rotate_delays();        //do something visible
            }
            sw_dly.set_ms( 100 );       //sw debounce (use for allswitches)
            //sw_dly sets while pressed
            //so will require sw release of 100ms
            //before sw_dly timer can expire
        }
        if( sw2.ison() ){
            if( sw_dly.isexpired() ){
                irq_blinkrate( 100 );   //++
            }
            sw_dly.set_ms( 100 );       //sw debounce (use for allswitches)
        }
        if( sw3.ison() ){
            if( sw_dly.isexpired() ){
                irq_blinkrate( -100 );  //--
            }
            sw_dly.set_ms( 100 );       //sw debounce (use for allswitches)
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

