#if 0
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
    //8N1 (is default, so really no need to set)
    info.mode ( info.MODE8N1 );
    info.on ( true );

    for(;;){
        info.puts ( "Hello World? " );
    }
}
#endif



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
    //C6 happens to not have analog function (no ansel), so no need to setup as pps overrides tris
    RPOR5bits.RP23R = 4; //RP23 -> U2TX
    U2MODEbits.BRGH = 1; //hispeed
    U2BRGbits.BRG = 26; //230400 w/24MHz 24MHz/4/230400=26.04
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

#if 1

//another uart test with irq

#include "Osc.hpp"
#include "Pins.hpp"
#include "Uart.hpp"
#include "Irq.hpp"
#include "Delay.hpp"
#include "Cp0.hpp"

//led status of uart- errors or activity with persist time
struct LedStatus {

    enum LEDSTATE { OK, OERR, BITERR, NOCHANGE };

    private:

    using led_t = struct {
        Pins pin;
        Delay persist;
        uint16_t ontime;
    };

    //rgb led on curiosity board
    led_t leds[3] = {
        { {Pins::D1,  Pins::OUT}, {}, 4000 },   //Red
        { {Pins::C3,  Pins::OUT}, {}, 100 },    //Green
        { {Pins::C15, Pins::OUT}, {}, 4000 }    //Blue
    };
    enum RGB { R, G, B };

    //led's state
    LEDSTATE state{NOCHANGE};

    public:

    //regular check/update from cp0 isr
    void update(){
        //check if persist time expired
        for( auto& n : leds ){
            if( n.persist.expired() ) n.pin.off();
        }

        //if no change to state, done
        if( state == NOCHANGE ) return;

        //change made, turn off all led's first
        for( auto& n : leds ) n.pin.off();

        //assume state is OK
        RGB n = G;
        if( state == OERR ) n = R;
        else if( state == BITERR ) n = B;

        //set delay and turn on
        leds[n].persist.set_ms( leds[n].ontime );
        leds[n].pin.on();
        state = NOCHANGE;
    }

    //set led status (from rx isr)
    void status(LEDSTATE s){ state = s; }

};
LedStatus leds;

//uart2 to Onion Omega2 uart1, 921600 baud (1Mbaud actual)
Uart info { Uart::UART2, Pins::C6, Pins::C7, 921600 };

int main()
{
    Osc osc;
    osc.pll_set( osc.MUL12, osc.DIV4 ); //24MHz
    info.on( true ); //turn on uart after osc (baud is set when turned on)
    Irq::init( Irq::UART2_RX, 1, 0, true ); //pri=1, spri=0, on=true
    Cp0::compare_ms( 10 ); //cp0 irq every 10ms
    Irq::init( Irq::CORE_TIMER, 1, 0, true );
    Irq::global( true );
    //simple markup for ansi-
    //  @+ = turn on markup
    //  @! = reset colors/attributes
    //  @W = white, @R = red, @G = gren, @B = blue (capitol letters = foreground)
    //  @w = white, @k = black (lowercase letters = background)
    //the uart inherits the Sprintf class, which does the printf work,
    //along with the markup decoding
    info.printf( "@+@!\r\n@WStarting uart echo test with @RL@GE@BD@W status...\r\n\r\n" );
    for(;;){
        Osc::idle(); //cp0 or rx2 will wake
    }
}

ISRautoflag( CORE_TIMER ){
    Cp0::compare_reload();
    leds.update();
}}

ISR( UART2_RX ){
    Irq irq;

    //get errors before reading rx buf
    if( info.rx_oerr() ){
        leds.status(leds.OERR);
        info.rx_oerrclr(); //clear all buffers
    } else if( info.rx_ferr() || info.rx_perr() ){
        leds.status(leds.BITERR);
        info.read(); //read just to advance buffer
    } else {
        leds.status(leds.OK);
        char c = info.read();
        info.putc( c ); //echo rx to tx
        if( c == '\r' ) info.putchar( '\n' ); //add nl to cr
        //Delay::wait_s( 2 ); //to force overrun error
    }

    irq.flag_clr( irq.UART2_RX );
}
#endif
