
#include <cstdint>
#include "Delay.hpp"
#include "Pins.hpp"
#include "Irq.hpp"
#include "Cp0.hpp"


Pins leds[4] = {
    { Pins::PORT::D, 3 },       //LED1
    { Pins::PORT::D, 1 },       //RED
    { Pins::PORT::C, 3 },       //GREEN
    { Pins::PORT::C, 15 }       //BLUE
};
Pins led2( Pins::PORT::C, 13 ); //LED2 (cp0 irq blinks)
Pins sw1( Pins::PORT::B, 9 );   //SW1 (rotate delays))

DelayCP0 sw_dly;                //debounce
DelayCP0 dly[4];                //CP0 led delays
uint32_t t_ms[4] = {            //led delay ms
    400,800,1000,1200
};

Irqn irq_cp0( Irq::CORE_TIMER, Irq::IRQ_PRI::PRI1, Irq::IRQ_SUB::SUB0 );


int main()
{
    //init pins
    sw1.digital(); sw1.pullup_on(); sw1.lowison();
    led2.digital(); led2.output();
    for( auto& led : leds ){
        led.output();
        led.digital();
    }

    //cp0 irq, set sysfreq, compare timeout, enable irq + global irqs
    Cp0::sysfreq = 24;
    Cp0::compare_ms( 250 );
    irq_cp0.enable();
    Irq::enable_all();

    //init delays or rotate delays
    auto rotate_delays = [ & ](){
        static uint8_t start = 0;
        for( auto& d : dly ){
            if( start >= 4 ) start &= 3;
            d.set_ms( t_ms[start++] );
        }
        start++;
    };
    //initialize delays
    rotate_delays();

    for (;;){
        for( auto i = 0; i < 4; i++ ){
            if( ! dly[i].isexpired() ) continue;
            leds[i].invert();
            dly[i].reset();
        }
        if( sw1.ison() ){
            if( sw_dly.isexpired() ){   //is a new press
                rotate_delays();        //do something visible
            }
            sw_dly.set_ms( 100 );       //sw debounce
            //sw_dly sets while pressed
            //so will require sw release of 100ms
            //before timer can expire
        }
    }
}

extern "C" {
    void __attribute__(( vector(0), interrupt(IPL1SOFT) )) CoreTimerISR()
    {
        Cp0::compare_reload();
        led2.invert();
        irq_cp0.flagclear();
    }
}

