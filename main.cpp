/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 testing - using C++

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/


#include <cstdint>
#include <stdio.h>

#include "Pins.hpp"
#include "Wdt.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Ccp.hpp"
#include "Uart.hpp"
#include "Cp0.hpp"
#include "Rtcc.hpp"
#include "Irq.hpp"


//svg colors for rgb led
const uint8_t svg[][3]{
///*aliceblue*/ {240,248,255},
///*antiquewhite*/ {250,235,215},
///*aqua*/ {0,255,255},
///*aquamarine*/ {127,255,212},
///*azure*/ {240,255,255},
///*beige*/ {245,245,220},
///*bisque*/ {255,228,196},
///*blanchedalmond*/ {255,235,205},
/*blue*/ {0,0,255},
///*blueviolet*/ {138,43,226},
///*brown*/ {165,42,42},
///*burlywood*/ {222,184,135},
///*cadetblue*/ {95,158,160},
///*chartreuse*/ {127,255,0},
///*chocolate*/ {210,105,30},
///*coral*/ {255,127,80},
///*cornflowerblue*/ {100,149,237},
///*cornsilk*/ {255,248,220},
///*crimson*/ {220,20,60},
///*cyan*/ {0,255,255},
///*darkblue*/ {0,0,139},
///*darkcyan*/ {0,139,139},
/*darkgoldenrod*/ {184,134,11},
///*darkgray*/ {169,169,169},
/*darkgreen*/ {0,100,0},
///*darkgrey*/ {169,169,169},
///*darkkhaki*/ {189,183,107},
/*darkmagenta*/ {139,0,139},
///*darkolivegreen*/ {85,107,47},
///*darkorange*/ {255,140,0},
///*darkorchid*/ {153,50,204},
/*darkred*/ {139,0,0},
///*darksalmon*/ {233,150,122},
/*darkseagreen*/ {143,188,143},
///*darkslateblue*/ {72,61,139},
///*darkslategray*/ {47,79,79},
///*darkslategrey*/ {47,79,79},
///*darkturquoise*/ {0,206,209},
/*darkviolet*/ {148,0,211},
/*deeppink*/ {255,20,147},
///*deepskyblue*/ {0,191,255},
///*dimgray*/ {105,105,105},
///*dimgrey*/ {105,105,105},
///*dodgerblue*/ {30,144,255},
///*firebrick*/ {178,34,34},
///*floralwhite*/ {255,250,240},
///*forestgreen*/ {34,139,34},
///*fuchsia*/ {255,0,255},
///*gainsboro*/ {220,220,220},
///*ghostwhite*/ {248,248,255},
///*gold*/ {255,215,0},
///*goldenrod*/ {218,165,32},
///*gray*/ {128,128,128},
///*green*/ {0,128,0},
///*greenyellow*/ {173,255,47},
///*grey*/ {128,128,128},
///*honeydew*/ {240,255,240},
/*hotpink*/ {255,105,180},
///*indianred*/ {205,92,92},
///*indigo*/ {75,0,130},
///*ivory*/ {255,255,240},
///*khaki*/ {240,230,140},
///*lavender*/ {230,230,250},
///*lavenderblush*/ {255,240,245},
///*lawngreen*/ {124,252,0},
///*lemonchiffon*/ {255,250,205},
///*lightblue*/ {173,216,230},
///*lightcoral*/ {240,128,128},
///*lightcyan*/ {224,255,255},
///*lightgoldenrodyellow*/ {250,250,210},
///*lightgray*/ {211,211,211},
///*lightgreen*/ {144,238,144},
///*lightgrey*/ {211,211,211},
///*lightpink*/ {255,182,193},
///*lightsalmon*/ {255,160,122},
///*lightseagreen*/ {32,178,170},
///*lightskyblue*/ {135,206,250},
///*lightslategray*/ {119,136,153},
///*lightslategrey*/ {119,136,153},
///*lightsteelblue*/ {176,196,222},
///*lightyellow*/ {255,255,224},
/*lime*/ {0,255,0},
///*limegreen*/ {50,205,50},
///*linen*/ {250,240,230},
/*magenta*/ {255,0,255},
///*maroon*/ {128,0,0},
///*mediumaquamarine*/ {102,205,170},
///*mediumblue*/ {0,0,205},
///*mediumorchid*/ {186,85,211},
///*mediumpurple*/ {147,112,219},
///*mediumseagreen*/ {60,179,113},
///*mediumslateblue*/ {123,104,238},
///*mediumspringgreen*/ {0,250,154},
///*mediumturquoise*/ {72,209,204},
///*mediumvioletred*/ {199,21,133},
///*midnightblue*/ {25,25,112},
///*mintcream*/ {245,255,250},
///*mistyrose*/ {255,228,225},
///*moccasin*/ {255,228,181},
///*navajowhite*/ {255,222,173},
/*navy*/ {0,0,128},
///*oldlace*/ {253,245,230},
///*olive*/ {128,128,0},
///*olivedrab*/ {107,142,35},
/*orange*/ {255,165,0},
///*orangered*/ {255,69,0},
///*orchid*/ {218,112,214},
///*palegoldenrod*/ {238,232,170},
///*palegreen*/ {152,251,152},
///*paleturquoise*/ {175,238,238},
///*palevioletred*/ {219,112,147},
///*papayawhip*/ {255,239,213},
///*peachpuff*/ {255,218,185},
///*peru*/ {205,133,63},
///*pink*/ {255,192,203},
///*plum*/ {221,160,221},
///*powderblue*/ {176,224,230},
/*purple*/ {128,0,128},
/*red*/ {255,0,0},
///*rosybrown*/ {188,143,143},
///*royalblue*/ {65,105,225},
///*saddlebrown*/ {139,69,19},
///*salmon*/ {250,128,114},
///*sandybrown*/ {244,164,96},
///*seagreen*/ {46,139,87},
///*seashell*/ {255,245,238},
///*sienna*/ {160,82,45},
///*silver*/ {192,192,192},
///*skyblue*/ {135,206,235},
///*slateblue*/ {106,90,205},
///*slategray*/ {112,128,144},
///*slategrey*/ {112,128,144},
///*snow*/ {255,250,250},
///*springgreen*/ {0,255,127},
///*steelblue*/ {70,130,180},
///*tan*/ {210,180,140},
///*teal*/ {0,128,128},
///*thistle*/ {216,191,216},
///*tomato*/ {255,99,71},
///*turquoise*/ {64,224,208},
///*violet*/ {238,130,238},
///*wheat*/ {245,222,179},
///*white*/ {255,255,255},
///*whitesmoke*/ {245,245,245},
/*yellow*/ {255,255,0},
///*yellowgreen*/ {154,205,50},
//
///*black*/ {0,0,0} //end
};


Uart info{Uart::UART2, Pins::C6, Pins::C7, 230400};


//printf - use replacement putc
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


//rgb led's struct, use pwm for brightness
//loop through svg colors by regularly calling update()
struct Rgb {

    Rgb(){
        //init pwm via loop- R,G use OCxB, B uses OCxE
        for(auto i = 0; i < 3; i++){
            m_ccp[i].mode(m_ccp[i].DEPWM16); //dual edge pwm 16bit
            m_ccp[i].compa(0);
            m_ccp[i].compb(0);
            m_ccp[i].out_pins(i == 2 ? Ccp::OCE : Ccp::OCB);
            m_ccp[i].on(true);
        }
    }

    //cycle through svg colors
    void update(){
        if(not m_delay.expired()) return;

        uint16_t t = m_delay_long;
        for(uint8_t i = 0; i < 3; i++){
            uint16_t v = m_ccp[i].compb();
            uint16_t s = svg[m_idx][i]<<8;
            if(v == s) continue;
            t = m_delay_short;
            if(v < s) v += 256; else v -= 256;
            m_ccp[i].compb(v);
        }
        m_delay.set_ms(t);
        if(t == m_delay_short) return;

        Rtcc::datetime_t dt = Rtcc::datetime();

        printf("$7color[$3%02d$7]: $2%03d.%03d.%03d$7",
            m_idx,m_ccp[0].compb()>>8,m_ccp[1].compb()>>8,m_ccp[2].compb()>>8);
        printf(" CP0 Count: $1%010u$7", Cp0::count());
        printf(" now: $5%02d-%02d-%04d %02d:%02d:%02d %s$7\r\n",
                dt.month, dt.day, dt.year+2000, dt.hour12, dt.minute, dt.second,
                dt.pm ? "PM" : "AM");

        if(++m_idx >= sizeof(svg)/sizeof(svg[0])) m_idx = 0;
    };

    private:

    static const uint16_t m_delay_short{10};
    static const uint16_t m_delay_long{1000};
    uint8_t m_idx{0};
    //pwm to rgb pins
    //mccp 1-3 pwm to rgb led's
    Ccp m_ccp[3]{ Ccp::CCP1, Ccp::CCP2, Ccp::CCP3 };

    Pins m_ledR{Pins::D1, Pins::OUT},
         m_ledG{Pins::C3, Pins::OUT},
         m_ledB{Pins::C15, Pins::OUT};

    Delay m_delay;

};



//alternate led/led2 at rate determined by pot via adc
//if very low value, turn off led's
struct Led12 {

    void update(){
        if(not m_delay.expired()) return;
        uint16_t t = m_pot.adcval()>>2; //(0-4096 -> 0-256)
        if(t < 100){
            t = 100;
            m_led1.off();
            m_led2.off();
        } else {
            m_led1_state = not m_led1_state;
            m_led1.latval(m_led1_state);
            m_led2.latval(not m_led1_state); //always opposite
        }
        m_delay.set_ms(t);
    };

    private:

    Pins m_pot{Pins::AN14}; //check pot val via adc
    Pins m_led1{Pins::D3, Pins::OUT};
    Pins m_led2{Pins::C13, Pins::OUT};
    bool m_led1_state{false};
    Delay m_delay;

};

extern "C" {
#include "usb.h"
}


int main()
{



    //just get/store resets cause (not used here,though)
    Resets::cause();

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    Rtcc::datetime_t dt = Rtcc::datetime();
    if(dt.year == 0) Rtcc::datetime( { 18, 3, 16, 0, 10, 32, 0} );

    Rtcc::on(true);

    info.on(true);
    cls(); //cls
    cursor(false); //hide cursor


//trying MCHP usb code
Delay::wait_s(2);
Irq::init(Irq::USB, 1, 0, false);
USBDeviceInit();
USBDeviceAttach();
Irq::global(true);
#include <stdio.h>




    Rgb rgb;
    Led12 led12;
    Delay dly;
    dly.set_ms(100);

    for(uint32_t i = 0;;i++){
        Wdt::reset(), led12.update(), rgb.update();

        if(USBUSARTIsTxTrfReady() && dly.expired())
        {
            char data[32];
            snprintf(data, 32, "%08X ",  USBGet1msTickCount());
            putsUSBUSART(data);
            dly.restart();
        }
        CDCTxService();
    }
}

