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



struct UsbTest {

    Pins sw3{ Pins::C4, Pins::INPU };   //turn usb on/off
    Pins sw2{ Pins::C10, Pins::INPU };  //enable/disable xmit data
    Pins led1{Pins::D3, Pins::OUT};     //led1 activity light

    UsbCdcAcm cdc;

    Delay sw_debounce, xmit_dly;

    bool xmit = false;

    void update(){
        if( sw3.ison() and sw_debounce.expired() ){
            cdc.init( not cdc.is_active() );
            sw_debounce.set_ms(500);
        }
        if( sw2.ison() and sw_debounce.expired() ){
            xmit = not xmit;
            sw_debounce.set_ms(500);
        }
        if( cdc.is_active() and xmit and xmit_dly.expired() ){
            xmit_dly.set_ms(1000);
            if(cdc.send((uint8_t*)"Hello ", strlen("Hello "))){
                led1.on();
                Delay::wait_ms( 20 );
                led1.off();
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

    printf("Starting...\r\n");

    for(;;){
        Wdt::reset();
        utest.update();
    }

}

