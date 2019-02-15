#include <cstdint>
#include "Pins.hpp"
#include "Osc.hpp"
#include "Irq.hpp"
#include "Timer23.hpp"

Timer23 t2{ Timer23::TMR2 };
Pins ledB{ Pins::C15, Pins::OUT};

int main()
{
    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);//8*12/4 = 24MHz

    Irq::isr_func( Irq::TIMER_3,
        []{
        ledB.invert();
        }
    );
    Irq::init( Irq::TIMER_3, 1, 0, true );
    Irq::global( true );

    t2.mode32( true);
    t2.period( 12000000 ); //24Mhz/2, 1Hz
    t2.on( true );
    for(;;){
        osc.idle();
    }
}

