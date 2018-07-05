#include "Osc.hpp"
#include "Uart.hpp"

int main()
{
    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);

    Uart info{Uart::UART2, Pins::C6, Pins::C7, 230400};
    info.mode(info.MODE8N1);
    info.on(true);

    for(;;){ info.putchar('U'); }
}

