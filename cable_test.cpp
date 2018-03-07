/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

  http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
  http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

  rj45 to rs232 cable tester

 =============================================================================*/

#include <cstdint>
#include <cstdlib>
#include <stdio.h>

#include "Pins.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Uart.hpp"
#include "Cp0.hpp"


//hook up serial for info output
Uart info{Uart::UART2, Pins::C6, Pins::C7, 230400};

//override printf putc- mchp only uses uart2
//although we are using uart2, this allows other uarts
extern "C" void _mon_putc(char c){ info.putc(c); }
void cls(){ info.putc(12); }

//press sw3 to test
Pins sw3{ Pins::C4,  Pins::INPU };

///struct to hold pin info
using pin_combo = struct { const char* name; uint8_t pn; Pins p; uint16_t v; };

//curiosity board to connectors
//J3 - rj45 - 1-8
//A14 B13 A7 B15 A12 B0 B2 C0
//J3 - rs232 - 2-8,20
//A15 A10 B14 A13 A11 A6 B1 B3

pin_combo pins[] = {
    //rj45
    //0-7 -> 1-8
    //1-8 shorted, 2-6-7 shorted, 3 4 5 to rs232 7,3,2
    { "A14 RJ45", 1, {Pins::A14, Pins::INPU}, 0b1111111101111110}, //1-8 shorted
    { "B13 RJ45", 2, {Pins::B13, Pins::INPU}, 0b1111111110011101}, //2-6-7 shorted
    { "A7  RJ45", 3, {Pins::A7,  Pins::INPU}, 0b1101111111111011}, //3- rs232 7
    { "B15 RJ45", 4, {Pins::B15, Pins::INPU}, 0b1111110111110111}, //4- rs232 3
    { "A12 RJ45", 5, {Pins::A12, Pins::INPU}, 0b1111111011101111}, //5- rs232 2
    { "B0  RJ45", 6, {Pins::B0,  Pins::INPU}, 0b1111111110011101}, //6-7-2 shorted
    { "B2  RJ45", 7, {Pins::B2,  Pins::INPU}, 0b1111111110011101}, //7-6-2 shorted
    { "C0  RJ45", 8, {Pins::C0,  Pins::INPU}, 0b1111111101111110}, //8-1 shorted

    //rs232
    //8-15 -> 2-8,20
    //2,3,7 from rj45 5,4,3, 4-5 shorted, 6-8-20 shorted
    { "A15 RS232", 2, {Pins::A15, Pins::INPU}, 0b1111111011101111}, //2- rj45 5
    { "A10 RS232", 3, {Pins::A10, Pins::INPU}, 0b1111110111110111}, //3- rj45 4
    { "B14 RS232", 4, {Pins::B14, Pins::INPU}, 0b1111001111111111}, //4-5 shorted
    { "A13 RS232", 5, {Pins::A13, Pins::INPU}, 0b1111001111111111}, //5-4 shorted
    { "A11 RS232", 6, {Pins::A11, Pins::INPU}, 0b0010111111111111}, //6-8-20 shorted
    { "A6  RS232", 7, {Pins::A6,  Pins::INPU}, 0b1101111111111011}, //7- rj45 3
    { "B1  RS232", 8, {Pins::B1,  Pins::INPU}, 0b0010111111111111}, //6-8-20 shorted
    { "B3  RS232",20, {Pins::B3,  Pins::INPU}, 0b0010111111111111}  //6-8-20 shorted
};

//read all pins, shift into 16bit number
uint16_t read_all(){
    auto ret = 0, shift = 0;
    for(auto& pc : pins){ ret |= pc.p.pinval()<<shift++; }
    return ret;
}

//check all pins, print info
bool check_all(){
    printf(
    "   RJ45 1-8 shorted, 2-6-7  shorted, 3,4,5 -> rs232 7,3,2  \r\n"
    "  RS232 4-5 shorted, 6-8-20 shorted, 7,3,2 -> rj45  3,4,5  \r\n\r\n"
    "-----------------------------------------------------------\r\n"
    "| each pin tested  |    RS232 Pins   |    RJ45 Pins       |\r\n"
    "| and all checked  ----------------------------------------\r\n"
    "| # = connected    |20 8 7 6 5 4 3 2 | 8 7 6 5 4 3 2 1    |\r\n"
    "-----------------------------------------------------------\r\n" );

    auto ret = true;            //assume all ok
    auto i = 0;
    for(auto& pc : pins){
        auto pinok = true;      //assume ok
        pc.p.digital_out();     //set pin to output
        pc.p.low();             //pull low
        Delay::wait_ms(10);     //give a little time
        auto r = read_all();    //read all pins
        if(r != pc.v){          //if something wrong on this pin
            ret = false;        //any pin fail, fail all test
            pinok = false;      //fail this pin
        }
        pc.p.digital_in();      //back to input
        Delay::wait_ms(10);     //wait a little

        //print this pin header info
        printf("[%02u] %-10s%-5d", i++, pc.name, pc.pn);
        //each pin info
        for(auto j = 15; j >= 0; j--){
            if(r & (1<<j)) printf(" -"); else printf("%2d", pins[j].pn);
            if(j != 8) continue;
            //now switching to pins on other side
            //if pins connected to 'other side'- print space, else |
            printf(" %s", ((r&0xFF00)==0xFF00 || (r&0xFF)==0xFF) ? "|" : " ");
        }
        //pin ok or fail
        printf("  %s\r\n", pinok ? "OK" : ":(" );
    }
    //print test result
    printf("\r\n=>%s!\r\n", ret ? "PASS" : "FAIL" );
    return ret;
}


int main()
{
    //set osc to 24MHz
    Osc::pll_set(Osc::MUL12, Osc::DIV4);  //8*12/4 = 24MHz

    //turn uart on after Osc set, so is using current
    //sysclock for baud_set (uart turn on will always set baud)
    info.on(true);
    cls();

    for(;;){
        while(not sw3.ison());
        //button pressed
        cls();
        check_all();
        Delay::wait_s(2);
    }
}

/*
  RS232 4-5 shorted, 6-8-20 shorted, 7,3,2 -> rj45  3,4,5
   RJ45 1-8 shorted, 2-6-7  shorted, 3,4,5 -> rs232 7,3,2

-----------------------------------------------------------
| each pin tested  |    RS232 Pins   |    RJ45 Pins       |
| and all checked  ----------------------------------------
| # = connected    |20 8 7 6 5 4 3 2 | 8 7 6 5 4 3 2 1    |
-----------------------------------------------------------
[00] A14 RJ45  1     - - - - - - - - | 8 - - - - - - 1  OK
[01] B13 RJ45  2     - - - - - - - - | - 7 6 - - - 2 -  OK
[02] A7  RJ45  3     - - 7 - - - - -   - - - - - 3 - -  OK
[03] B15 RJ45  4     - - - - - - 3 -   - - - - 4 - - -  OK
[04] A12 RJ45  5     - - - - - - - 2   - - - 5 - - - -  OK
[05] B0  RJ45  6     - - - - - - - - | - 7 6 - - - 2 -  OK
[06] B2  RJ45  7     - - - - - - - - | - 7 6 - - - 2 -  OK
[07] C0  RJ45  8     - - - - - - - - | 8 - - - - - - 1  OK
[08] A15 RS232 2     - - - - - - - 2   - - - 5 - - - -  OK
[09] A10 RS232 3     - - - - - - 3 -   - - - - 4 - - -  OK
[10] B14 RS232 4     - - - - 5 4 - - | - - - - - - - -  OK
[11] A13 RS232 5     - - - - 5 4 - - | - - - - - - - -  OK
[12] A11 RS232 6    20 8 - 6 - - - - | - - - - - - - -  OK
[13] A6  RS232 7     - - 7 - - - - -   - - - - - 3 - -  OK
[14] B1  RS232 8    20 8 - 6 - - - - | - - - - - - - -  OK
[15] B3  RS232 20   20 8 - 6 - - - - | - - - - - - - -  OK

=> PASS!

*/

