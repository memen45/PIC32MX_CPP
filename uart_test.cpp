#include "Osc.hpp"
#include "Uart.hpp"

//xc32 v2.10, optimizations -Os , each function in section, remove unused sections
//data used = 72 bytes
//flash used = 4810 bytes

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




#if 0

//here is a C version, doing minimal to get equivalent function as C++ code above

//xc32 v2.10, optimizations -Os , each function in section, remove unused sections
//data used = 0 bytes
//flash used = 2390 bytes
//also note that in the C++ version, there is more functionality brought in-
//osc will keep track of cpu speed
//baud will be calculated automatically (based on actual clock speeds)
//syskey unlock/lock is more robust (irq/dma accounted for)
//pin setup is done automatically- in/out/pullup/pps/etc

#include <xc.h>

void main(void) {

    //osc to 24mhz, FRC as source
    SYSKEY = 0xAA996655; //syskey unlock
    SYSKEY = 0x556699AA;
    OSCCONbits.NOSC = 0; //switch to frc before changing pll
    OSCCONbits.OSWEN = 1; //enable switch
    while(OSCCONbits.OSWEN); //wait till done
    SPLLCONbits.PLLMULT = 5; //x12
    SPLLCONbits.PLLODIV = 2; // /4
    SPLLCONbits.PLLICLK = 1; //spll src=frc
    OSCCONbits.NOSC = 1; //switch to spll
    OSCCONbits.OSWEN = 1; //enable switch
    while(OSCCONbits.OSWEN); //wait till done
    SYSKEY = 0; //syskey lock

    //uart2 tx->pin C6 (RP23), 230400 baud, 8N1
    RPOR5bits.RP23R = 4; //RP23 -> U2TX
    U2MODEbits.BRGH = 1; //hispeed
    U2BRGbits.BRG = 2604; //230400 w/24MHz
    U2MODEbits.PDSEL = 0; //8N
    U2MODEbits.STSEL = 0; //1
    U2STAbits.UTXEN = 1; //tx on
    U2MODEbits.ON = 1; //uart on

    const char hw[] = "Hello World? ";
    for( ; ; ){
        const char* p = hw;
        for(; *p; p++){
            while(U2STAbits.UTXBF); //wait if buffer full
            U2TXREG = *p; //send
        }
    }
}

#endif
