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

struct Svg {

    static uint8_t R(size_t);
    static uint8_t G(size_t);
    static uint8_t B(size_t);
    static const char* name(size_t);
    static bool valid(size_t);
    static uint8_t randcolorn();

    private:
    using svg_t = struct { const char* name; uint32_t rgbval; };
    static const svg_t m_svg[];
};

const Svg::svg_t Svg::m_svg[] = {
    {"alice blue",0xF0F8FF},
    {"antique white",0xFAEBD7},
    {"aqua",0x00FFFF},
    {"aquamarine",0x7FFFD4},
    {"azure",0xF0FFFF},
    {"beige",0xF5F5DC},
    {"bisque",0xFFE4C4},
    {"black",0x000000},
    {"blanched almond",0xFFEBCD},
    {"blue",0x0000FF},
    {"blue violet",0x8A2BE2},
    {"brown",0xA52A2A},
    {"burly wood",0xDEB887},
    {"cadet blue",0x5F9EA0},
    {"chartreuse",0x7FFF00},
    {"chocolate",0xD2691E},
    {"coral",0xFF7F50},
    {"cornflower blue",0x6495ED},
    {"cornsilk",0xFFF8DC},
    {"crimson",0xDC143C},
    {"cyan",0x00FFFF},
    {"dark blue",0x00008B},
    {"dark cyan",0x008B8B},
    {"dark golden rod",0xB8860B},
    {"dark gray",0xA9A9A9},
    {"dark green",0x006400},
    {"dark khaki",0xBDB76B},
    {"dark magenta",0x8B008B},
    {"dark olive green",0x556B2F},
    {"dark orange",0xFF8C00},
    {"dark orchid",0x9932CC},
    {"dark red",0x8B0000},
    {"dark salmon",0xE9967A},
    {"dark sea green",0x8FBC8F},
    {"dark slate blue",0x483D8B},
    {"dark slate gray",0x2F4F4F},
    {"dark turquoise",0x00CED1},
    {"dark violet",0x9400D3},
    {"deep pink",0xFF1493},
    {"deep sky blue",0x00BFFF},
    {"dim gray",0x696969},
    {"dodger blue",0x1E90FF},
    {"fire brick",0xB22222},
    {"floral white",0xFFFAF0},
    {"forest green",0x228B22},
    {"fuchsia",0xFF00FF},
    {"gainsboro",0xDCDCDC},
    {"ghost white",0xF8F8FF},
    {"gold",0xFFD700},
    {"golden rod",0xDAA520},
    {"gray",0x808080},
    {"green",0x008000},
    {"green yellow",0xADFF2F},
    {"honey dew",0xF0FFF0},
    {"hot pink",0xFF69B4},
    {"indian red",0xCD5C5C},
    {"indigo",0x4B0082},
    {"ivory",0xFFFFF0},
    {"khaki",0xF0E68C},
    {"lavender",0xE6E6FA},
    {"lavender blush",0xFFF0F5},
    {"lawn green",0x7CFC00},
    {"lemon chiffon",0xFFFACD},
    {"light blue",0xADD8E6},
    {"light coral",0xF08080},
    {"light cyan",0xE0FFFF},
    {"light golden rod yellow",0xFAFAD2},
    {"light gray",0xD3D3D3},
    {"light green",0x90EE90},
    {"light pink",0xFFB6C1},
    {"light salmon",0xFFA07A},
    {"light sea green",0x20B2AA},
    {"light sky blue",0x87CEFA},
    {"light slate gray",0x778899},
    {"light steel blue",0xB0C4DE},
    {"light yellow",0xFFFFE0},
    {"lime",0x00FF00},
    {"lime green",0x32CD32},
    {"linen",0xFAF0E6},
    {"magenta",0xFF00FF},
    {"maroon",0x800000},
    {"medium aquamarine",0x66CDAA},
    {"medium blue",0x0000CD},
    {"medium orchid",0xBA55D3},
    {"medium purple",0x9370DB},
    {"medium sea green",0x3CB371},
    {"medium slate blue",0x7B68EE},
    {"medium spring green",0x00FA9A},
    {"medium turquoise",0x48D1CC},
    {"medium violet red",0xC71585},
    {"midnight blue",0x191970},
    {"mint cream",0xF5FFFA},
    {"misty rose",0xFFE4E1},
    {"moccasin",0xFFE4B5},
    {"navajo white",0xFFDEAD},
    {"navy",0x000080},
    {"old lace",0xFDF5E6},
    {"olive",0x808000},
    {"olive drab",0x6B8E23},
    {"orange",0xFFA500},
    {"orange red",0xFF4500},
    {"orchid",0xDA70D6},
    {"pale golden rod",0xEEE8AA},
    {"pale green",0x98FB98},
    {"pale turquoise",0xAFEEEE},
    {"pale violet red",0xDB7093},
    {"papaya whip",0xFFEFD5},
    {"peach puff",0xFFDAB9},
    {"peru",0xCD853F},
    {"pink",0xFFC0CB},
    {"plum",0xDDA0DD},
    {"powder blue",0xB0E0E6},
    {"purple",0x800080},
    {"rebecca purple",0x663399},
    {"red",0xFF0000},
    {"rosy brown",0xBC8F8F},
    {"royal blue",0x4169E1},
    {"saddle brown",0x8B4513},
    {"salmon",0xFA8072},
    {"sandy brown",0xF4A460},
    {"sea green",0x2E8B57},
    {"sea shell",0xFFF5EE},
    {"sienna",0xA0522D},
    {"silver",0xC0C0C0},
    {"sky blue",0x87CEEB},
    {"slate blue",0x6A5ACD},
    {"slate gray",0x708090},
    {"snow",0xFFFAFA},
    {"spring green",0x00FF7F},
    {"steel blue",0x4682B4},
    {"tan",0xD2B48C},
    {"teal",0x008080},
    {"thistle",0xD8BFD8},
    {"tomato",0xFF6347},
    {"turquoise",0x40E0D0},
    {"violet",0xEE82EE},
    {"wheat",0xF5DEB3},
    {"white",0xFFFFFF},
    {"white smoke",0xF5F5F5},
    {"yellow",0xFFFF00},
    {"yellow green",0x9ACD32}
};
uint8_t Svg::R(size_t i){
    if( not valid(i) ) return 0;
    return ( m_svg[i].rgbval>>16) bitand 0xff;
}
uint8_t Svg::G(size_t i){
    if( not valid(i) ) return 0;
    return ( m_svg[i].rgbval>>8) bitand 0xff;
}
uint8_t Svg::B(size_t i){
    if( not valid(i) ) return 0;
    return m_svg[i].rgbval bitand 0xff;
}
const char* Svg::name(size_t i){
    if( not valid(i) ) return 0;
    return Svg::m_svg[i].name;
}
bool Svg::valid(size_t i){
    return i < size(m_svg);
}
uint8_t Svg::randcolorn(){
    return rand() % size(m_svg);
}


//UsbDebug will use, will also be default for printf
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
        uint16_t pvR = m_ccp[0].compb()>>8; //get pwm value for R
        uint16_t pvG = m_ccp[1].compb()>>8; //get pwm value for G
        uint16_t pvB = m_ccp[2].compb()>>8; //get pwm value for B
        uint8_t sR = Svg::R(m_svgn); //svg color value R
        uint8_t sG = Svg::G(m_svgn); //svg color value G
        uint8_t sB = Svg::B(m_svgn); //svg color value B

        auto check = [&](uint16_t pv, uint8_t s, uint8_t cn){
            if( pv != s ){
                pv += pv < s ? 1 : -1;
                t = m_delay_short;
                m_ccp[cn].compb(pv<<8);
            }
        };
        check(pvR, sR, 0);
        check(pvG, sG, 1);
        check(pvB, sB, 2);

        m_delay.set(t);
        if(t == m_delay_short) return;

        if( not UsbDebug::debug() ){ //if not using uart for debug, print this
        Rtcc::datetime_t dt = Rtcc::datetime();
        info.printf("{W}[{R}%03d{W}] {/}%-25s {|G}%03d.%03d.%03d{W}",
            m_svgn, Svg::name(m_svgn), sR, sG, sB);
        info.printf(" CP0 Count: {B}%010u{W}", Cp0::count());
        info.printf(" now: {M}%02d-%02d-%04d %02d:%02d:%02d %s{W}\r\n",
                dt.month, dt.day, dt.year+2000, dt.hour12, dt.minute, dt.second,
                dt.pm ? "PM" : "AM");
        }

        //if( not Svg::valid(++m_svgn) ) m_svgn = 0;
        m_svgn = Svg::randcolorn();

    };

    private:

    static const uint32_t m_delay_short{ 10_ms };
    static const uint32_t m_delay_long{ 50_ms };
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
        dly.set(1_sec);
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

    //+ turn on markup
    //! reset colors/attributes/cls/home
    info.printf("{+!Y}\r\nTesting {/M_}printf{|Y} from uart{W}\r\n");

    //enable usb debugging via uart
    UsbDebug dbg;
    dbg.debug( &info ); //set UsbDebug to use our uart
    dbg.debug("{G}Testing {/M_}printf{|G} from debug{W}\r\n");

    Rgb rgb;
    Led12 led12;

    Pins pot{Pins::AN14}; //test adc, get pot adc val
    uint32_t p = pot.adcval(); //12bit
    p = p*1000 / 4095; // % x10

    dbg.debug("{C}pot value: {R}%d.%d%%{W}\r\n", p/10, p%10);
    dbg.debug("{B}UDID: %016llx\r\n", Sys::udid()); //check udid
    dbg.debug("{G}starting...\r\n{W}");

    for(;;){
        Wdt::reset(); //in case fused on
        led12.update();
        rgb.update();
        utest.update();
    }

}


