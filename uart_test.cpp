#include "Osc.hpp"
#include "Uart.hpp"

int main()
{
    //set osc to 24MHz
    Osc osc;
    osc.pll_set ( osc.MUL12, osc.DIV4 );
    //tx only
    Uart info { Uart::UART2TX, Pins::C6, 230400 };
    //8N1 (is default, so really not needed to be set)
    info.mode ( info.MODE8N1 );
    info.on ( true );

    for(;;){ 
        info.puts ( "Hello World? " );
    }
}

