
#include <cstdint>
#include "Delay.hpp"
#include "Pins.hpp"
#include "Irq.hpp"

#define CP0_SET_COMPARE(v)  __builtin_mtc0(11,0,v)
#define CP0_GET_COUNT()     __builtin_mfc0(9,0)

Pins leds[5] = {
    { Pins::PORT::D, 3 },       //LED1
    { Pins::PORT::C, 13 },      //LED2
    { Pins::PORT::D, 1 },       //RED
    { Pins::PORT::C, 3 },       //GREEN
    { Pins::PORT::C, 15 }       //BLUE
};
Pins sw1( Pins::PORT::B, 9 );   //SW1
DelayCP0 sw_dly;                //debounce
DelayCP0 dly[5];                //CP0 led delays
uint32_t t_ms[5] = {            //led delay ms
    200,400,600,800,1000
};

Irqn irq_cp0( Irq::CORE_TIMER, Irq::IRQ_PRI::PRI1, Irq::IRQ_SUB::SUB0 );

int main()
{
    sw1.digital(); sw1.pullup_on(); sw1.lowison();
//    irq_cp0.enable();
//    CP0_SET_COMPARE( CP0_GET_COUNT() + 12000000UL );
//    __builtin_enable_interrupts();
    //init delays, pins, also use to rotate delays
    auto rotate_delays = [ & ](){
        static uint8_t start = 0;
        for( auto i = 0; i < 4; i++ ){
            dly[i].set_ms( t_ms[start++] );
            leds[i].output(); //also set output
            leds[i].digital(); //and digital- no harm doing again
            start = start == 4 ? 0 : start;
        }
        start = ++start == 4 ? 0 : start;
    };

    rotate_delays(); //initial set delays, pin outputs

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

#ifdef __cplusplus
extern "C" {
#endif
    void __attribute__(( vector(0), interrupt(IPL1SOFT) )) CoreTimerISR(){
        CP0_SET_COMPARE( CP0_GET_COUNT() + 12000000UL );
        leds[5].invert();
        irq_cp0.flagclear();
    }
#ifdef __cplusplus
}
#endif

