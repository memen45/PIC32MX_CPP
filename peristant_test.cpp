//persistant data test
//pic32mm0256gpm064
//xc32 2.15

#include <cstdint>

#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Uart.hpp"

Uart info{Uart::UART2, Pins::C6, Pins::C7, 921600};

unsigned int __attribute__((persistent)) pattern;

int main()
{
    uint8_t r = (uint8_t)Resets::cause();
    /*
                enum
    CAUSE       : uint8_t {
                EXTR = 1<<7, SWR = 1<<6, //1<<5 none- reads 0
                WDTO = 1<<4, SLEEP = 1<<3, IDLE = 1<<2, BOR = 1<<1, POR = 1<<0
                };
    */
   const char* causes[] = {
       "POR","BOR","IDLE","SLEEP","WDTO","SOMETHING WRONG","SWR","EXTR"
   };

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc
    //info uart on
    info.on( true );
    //print pattern value and reset cause
    info.printf( "\r\n\r\npattern: %d\r\n", pattern++ );
    info.printf( "Reset Cause: 0x%02X - ", r );
    for(auto i = 0; i < 8; i++){
        if( r bitand (1<<i) ) {
            info.printf( "%s ", causes[i] );
        }
    }
    //delay
    info.printf( "\r\nDelay 2 seconds...");
    Delay::wait( 2_sec );
    info.printf( "done. Resetting...");
    Delay::wait( 100_ms ); //wait for chars to make it out
    //inc pattern 0-9
    if( pattern > 9 ) pattern = 0;
    //swreset
    Resets::swreset();
    for(;;){}
};

/*
pattern: -14663259
Reset Cause: 0x80 - EXTR
Delay 2 seconds...done. Resetting...

pattern: 0
Reset Cause: 0x40 - SWR
Delay 2 seconds...done. Resetting...

pattern: 1
Reset Cause: 0x40 - SWR
Delay 2 seconds...done. Resetting...

//2 3 4 5 6 7

pattern: 8
Reset Cause: 0x40 - SWR
Delay 2 seconds...done. Resetting...

pattern: 9
Reset Cause: 0x40 - SWR
Delay 2 seconds...done. Resetting...

pattern: 0
Reset Cause: 0x40 - SWR
Delay 2 seconds...done. Resetting...
*/