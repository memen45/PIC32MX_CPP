/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "Pins.hpp"
#include "Wdt.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Uart.hpp"
#include "UsbCdcAcm.hpp"
#include "UsbDebug.hpp"


//need uart for usb debugging
Uart info{Uart::UART2, Pins::C6, Pins::C7, 1000000};

//pins used
Pins sw3{ Pins::C4, Pins::INPU };   //turn usb on/off
Pins sw2{ Pins::C10, Pins::INPU };  //enable/disable xmit data
Pins sw1{ Pins::B9, Pins::INPU };   //enable/disable usb debug output
Pins led2{ Pins::C13, Pins::OUT };  //led2 usb active light
Pins led1{ Pins::D3, Pins::OUT };   //led1 activity light

struct UsbTest {

    private:

    UsbCdcAcm cdc;
    UsbDebug dbg;
    Delay xmit_dly;
    bool xmit = false;

    //demostrate usb tx callback
    static bool txcallback(UsbEP* ep){
        Delay::wait(3_ms);
        led1.off();
        return true;
    }

    void debounce(Pins& p){
        Delay::wait( 50_ms ); //down
        while( p.ison() );
        Delay::wait( 100_ms ); //up, long
    }

    public:

    void update(){
        if( sw3.ison() ){
            cdc.init( not cdc.is_active() );
            debounce( sw3 );
        }
        if( not cdc.is_active() ){
            led2.off();
            xmit = false;
            xmit_dly.expire();
            return; //not on, no need to go further
        }
        led2.on(); //usb is on
        if( sw2.ison() ){
            xmit = not xmit; //toggle xmit message
            debounce( sw2 );
        }
        if( sw1.ison() ){
            dbg.debug( dbg.debug() ); //toggle debug
            debounce( sw2 );
        }
        if( xmit and xmit_dly.expired() ){
            xmit_dly.sets(500_ms); //delay between sending
            //cycle through ansi colors 1-7
            static char hw[] = "\033[3_mHello ";
            static int color = 1;
            hw[3] = color + '0'; //replace _ with 1-7
            color++; if( color > 7 ) color = 1;
            //send, turn on led1, callback will turn it off
            if( cdc.write((const char*)hw, txcallback) ){
                led1.on();
            }
        }
    }
};
UsbTest utest;


int main()
{
    //get/store reset cause
    Resets::cause();

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);   //8*12/4 = 24MHz (mul12 for usb)
    osc.sosc(true);                     //enable sosc if not already
    osc.tun_auto(true);                 //let sosc tune frc

    info.on(true);
    UsbDebug::debug( &info );

    info.printf("\r\n\r\nStarting...\r\n");

    for(;;){
        Wdt::reset();
        utest.update();
    }

}

