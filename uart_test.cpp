#include "Osc.hpp"
#include "Uart.hpp"

int main()
{
    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);
    //tx only
    Uart info{Uart::UART2TX, Pins::C6, 230400};
    info.on(true);

    for(;;){ 
        info.puts("Hello World? ");
    }
}

