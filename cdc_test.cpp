/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 testing - using C++

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/
#include <cstdint>
#include <cstring> //strlen
#include <cstdio>
#include <cstdlib>

#include "Pins.hpp"
#include "Wdt.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Uart.hpp"
#include "UsbCdcAcm.hpp"
#include "UsbEP.hpp" //notify_t


//need uart going for usb debugging
Uart info{Uart::UART2, Pins::C6, Pins::C7, 1000000};

//printf - use replacement putc so we can use our uart class
//will use $ for trigger to print ansi colors (use $$ if want $ character)
//printf("this is $1red $7white and I have $$1");
extern "C" void _mon_putc(char c){
    static bool trigger = false;
    if(trigger){
        trigger = false;
        if(c >= '0' && c <= '7'){
            info.puts("\033[3");    //ansi color start
            info.putchar(c);        //plus color
            c = 'm';                //below will output this
        }
        info.putchar(c);            //'m' from above, or regular char after '$'
        return;
    }
    //not triggered
    if(c == '$') trigger = true;//trigger char
    else info.putchar(c);       //regular char
}

void cls(){ info.putchar(12); }
void cursor(bool tf){ printf("\033[?25%c", tf ? 'h' : 'l'); }
void ansi_reset(){ printf("\033[0m"); }

//keep them 'global' (led1 used outside of class)
Pins sw3{ Pins::C4, Pins::INPU };   //turn usb on/off
Pins sw2{ Pins::C10, Pins::INPU };  //enable/disable xmit data
Pins led2{ Pins::C13, Pins::OUT };  //led2 usb active light
Pins led1{ Pins::D3, Pins::OUT };   //led1 activity light

struct UsbTest {

    UsbCdcAcm cdc;

    Delay xmit_dly;

    bool xmit = false;

    static bool txcallback(UsbEP* ep){
        Delay::wait_ms(3);
        led1.off();
        return true;
    }

    void update(){
        if( sw3.ison() ){
            cdc.init( not cdc.is_active() );
            //debounce
            while( sw3.ison() );
            Delay::wait_ms(100);
        }
        if( not cdc.is_active() ){
            led2.off();
            xmit = false;
            xmit_dly.expire();
            return; //not on, no need to go further
        }
        led2.on();
        if( sw2.ison() ){
            xmit = not xmit;
            //debounce
            while( sw2.ison() );
            Delay::wait_ms(100);
        }
        if( xmit and xmit_dly.expired() ){
            xmit_dly.set_ms(500);
            //cycle through ansi colors 1-7
            static char hw[] = "\033[3_mHello ";
            static int count = 1;
            hw[3] = count + '0';
            count++; if( count > 7 ) count = 1;
            //send, turn on led1, callback will turn it off
            if( cdc.send((const char*)hw, txcallback) ){
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
    cls(); //cls
    cursor(false); //hide cursor

    printf("\r\n\r\nStarting...\r\n");

    for(;;){
        Wdt::reset();
        utest.update();
    }

}

