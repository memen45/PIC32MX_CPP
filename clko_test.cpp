#include <cstdint>
#include "Pins.hpp"
#include "Osc.hpp"
#include "Timer23.hpp"
#include "Delay.hpp"
#include "Uart.hpp"

//count CLKO pulses in 1 second
//(assumes clock set correctly, as it is our timebase)
//jumper Pin26/CLKO/RA3 <-> pin19/T2CK/RC0
//config bits - #pragma config OSCIOFNC = ON

//timer2 in 32bit mode, clocked by CLKO
Timer23 t2{ Timer23::TMR2 };
Timer23 t3{ Timer23::TMR3 }; //need for setting clk source
//uart tx info
Uart info { Uart::UART2TX, Pins::C6, 921600 };

int main()
{
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);//8*12/4 = 24MHz
    info.on ( true );       //uart on
    t2.mode32( true);       //timer2 in 32bit mode
    t3.clk_src( t3.T2CK );  //clocked from clko (clk src set in timer3)
    t2.on( true );          //start timer2
    Delay::wait( 1_sec );   //wait 1 second
    t2.on( false );         //turn off timer2

    info.printf( "Timer2 counts: %d\r\n", t2.count() );

    /*
    result-
    Timer2 counts: 24000092
    */
    for(;;){
        osc.idle();
    }
}

