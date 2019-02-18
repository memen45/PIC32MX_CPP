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
#include "Ccp.hpp"
#include "Uart.hpp"
#include "Cp0.hpp"
#include "Rtcc.hpp"
#include "Irq.hpp"
#include "Sys.hpp"
#include "UsbCdcAcm.hpp"
#include "Nvm.hpp"
#include "UsbDebug.hpp"
#include "Util.hpp"


//svg colors for rgb led
const uint8_t svg[][3]{
/*blue*/ {0,0,255},
/*darkgoldenrod*/ {184,134,11},
/*darkgreen*/ {0,100,0},
/*darkmagenta*/ {139,0,139},
/*darkred*/ {139,0,0},
/*darkseagreen*/ {143,188,143},
/*darkviolet*/ {148,0,211},
/*deeppink*/ {255,20,147},
/*hotpink*/ {255,105,180},
/*lime*/ {0,255,0},
/*magenta*/ {255,0,255},
/*navy*/ {0,0,128},
/*orange*/ {255,165,0},
/*purple*/ {128,0,128},
/*red*/ {255,0,0},
/*yellow*/ {255,255,0},
};


//UsbDebug will use, will also be deault for printf
//(via _mon_putc in UsbDeg.cpp)
Uart info{Uart::UART2, Pins::C6, Pins::C7, 921600};


//rgb led's struct, use pwm for brightness
//loop through svg colors by regularly calling update()
struct Rgb {

    Rgb(){
        //init pwm via loop- R,G use OCxB, B uses OCxE
        for(auto& m : m_ccp){
            m.mode(m.DEPWM16); //dual edge pwm 16bit
            m.compa(0);
            m.compb(0);
            m.out_pins(m.ccp_num() == 3 ? m.OCE : m.OCB);
            m.on(true);
        }
    }

    //cycle through svg colors
    void update(){
        if(not m_delay.expired()) return;

        uint32_t t = m_delay_long;
        //adjust pwm to match new colors (short delay per adjustment)
        for(size_t i = 0; i < 3; i++){
            uint16_t v = m_ccp[i].compb(); //get pwm value
            uint16_t s = svg[m_svgn][i]<<8; //and svg color value
            if(v == s) continue; //color is now set
            t = m_delay_short; //ned to adjust more
            if(v < s) v += 256; else v -= 256; //inc/dec color toward match
            m_ccp[i].compb(v); //set new value
        }
        m_delay.set(t);
        if(t == m_delay_short) return;

        Rtcc::datetime_t dt = Rtcc::datetime();

        if( not UsbDebug::debug() ){ //if not using uart for debug, print this
        info.printf("@Wcolor[@R%02d@W]: @G%03d.%03d.%03d@W",
            m_svgn,m_ccp[0].compb()>>8,m_ccp[1].compb()>>8,m_ccp[2].compb()>>8);
        info.printf(" CP0 Count: @B%010u@W", Cp0::count());
        info.printf(" now: @M%02d-%02d-%04d %02d:%02d:%02d %s@W\r\n",
                dt.month, dt.day, dt.year+2000, dt.hour12, dt.minute, dt.second,
                dt.pm ? "PM" : "AM");
        }

        if( ++m_svgn >= size(svg) ) m_svgn = 0;

    };

    private:

    static const uint32_t m_delay_short{ 10_ms };
    static const uint32_t m_delay_long{ 1_sec };
    uint8_t m_svgn{ 0 };
    //pwm to rgb pins
    //mccp 1-3 pwm to rgb led's
    Mccp m_ccp[3]{ Mccp::CCP1, Mccp::CCP2, Mccp::CCP3 };

    Delay m_delay;

};



//alternate led/led2 at rate determined by pot via adc
//if very low value, turn off led's
struct Led12 {

    void update(){
        if(not m_delay.expired()) return;
        uint32_t t = 1_ms * (m_pot.adcval()>>2); //(0-4096 -> 0-256)
        if(t < 100_ms){
            t = 100_ms;
            m_led1.off();
            m_led2.off();
        } else {
            m_led1.invert();
            m_led2.latval( not m_led1.latval() ); //always opposite
        }
        m_delay.set(t);
    };

    private:

    Pins m_pot{Pins::AN14}; //check pot val via adc
    Pins m_led1{Pins::D3, Pins::OUT};
    Pins m_led2{Pins::C13, Pins::OUT};
    Delay m_delay;

};

struct UsbTest {

    Pins sw3{ Pins::C4, Pins::INPU }; //turn on/off usb
    Pins sw1{ Pins::B9, Pins::INPU }; //enable/disable debug
    Pins sw2{ Pins::C10, Pins::INPU }; //xmit data

    UsbCdcAcm cdc;

    Delay dly;

    char buf[64];
    bool ison = false;
    bool xmit = false;

    void debounce(Pins& p){
        Delay::wait( 50_ms ); //down
        while( p.ison() );
        Delay::wait( 100_ms ); //up (long ok here)
    }

    bool notify(UsbEP* ep){
        static uint8_t count = 1;
        dly.set(1000_ms);
        if(count > 231) count = 1;
        Rtcc::datetime_t dt = Rtcc::datetime();

        cdc.printf("\033[38;5;%dm[%02d] %02d-%02d-%04d %02d:%02d:%02d %s ",
                count,
                count,
                dt.month, dt.day, dt.year+2000, dt.hour12, dt.minute, dt.second,
                dt.pm ? "PM" : "AM");
        count++;
        return true;
    }

    void update(){
        if( sw3.ison() ){
            cdc.init( not cdc.is_active() );
            debounce( sw3 );
        }
        if( not cdc.is_active() ) return;
        if( sw1.ison() ){
            UsbDebug::debug( not UsbDebug::debug() );
            debounce( sw1 );
        }
        if( sw2.ison() ){
            xmit = not xmit;
            debounce( sw2 );
        }
        if( xmit and dly.expired()) notify(0);
    }

};
UsbTest utest;


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
    if(dt.year == 0) Rtcc::datetime( {
        // 19, 1, 29, 0, 9, 45, 0 -> Y,M,D,0,H,M.S (day calculated in Rtcc)
        //add to pre-build step
        //date +%-y,%-m,%-d,0,%-k,%-M,%-S > ${ProjectDir}/now.h
        //so date/time automatically set
        #include "now.h"
        //will be a few seconds lagging- compile time+programming time
    } );

    Rtcc::on(true);
    info.on(true);  //uart on

    //@+ turn on markup
    //@! reset colors/attributes/cls/home
    info.printf("@+@!@Y\r\nTesting @Mprintf@Y from uart@W\r\n");

    //enable usb debugging via uart
    UsbDebug dbg;
    dbg.debug( &info ); //set UsbDebug to use our uart
    dbg.debug("@GTesting @Mprintf@G from debug@W\r\n");

    Rgb rgb;
    Led12 led12;

    Pins pot{Pins::AN14}; //test adc, get pot adc val
    uint32_t p = pot.adcval(); //12bit
    p = p*1000 / 4095; // % x10

    dbg.debug("@Cpot value: @R%d.%d%%@W\r\n", p/10, p%10);
    dbg.debug("@BUDID: %016llx\r\n", Sys::udid()); //check udid
    dbg.debug("@Gstarting...\r\n@W");

    for(;;){
        Wdt::reset(); //in case fused on
        led12.update();
        rgb.update();
        utest.update();
    }

}


