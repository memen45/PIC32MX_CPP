/*
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 stepper motor test

 stepper motor - 17HS13-0404S1 (stepperonline, via ebay)

 stepper controller - Toshiba TB6612FNG (sparkfun board)

 stdby, pwma, pwmb - all tied to vcc
 vm - using 9v battery

*/

#include <cstdint>

#include "Pins.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Uart.hpp"

Uart info{Uart::UART2, Pins::C6, Pins::C7, 1000000};
Pins sw3{ Pins::C4, Pins::INPU }; //enable/disable stepper test
Pins pot{ Pins::AN14 }; //stepper speed, check pot val via adc
Pins stepper_pins[4]{
    {Pins::C0, Pins::OUT}, //AI1
    {Pins::C2, Pins::OUT}, //AI2
    {Pins::B0, Pins::OUT}, //BI1
    {Pins::B2, Pins::OUT}, //BI2
};

struct StepperDriver {

    StepperDriver( Pins (&pins)[4] ) : m_pins(pins) {}

    void stop(void){
        for( auto& p : m_pins ) p.off();
    }

    void brake(void){
        //brake - short one coil together (high in this case)
        //via both AI pins enabled (since pwm is tied high)
        m_pins[0].on();
        m_pins[1].on();
    }

    void step(int n, uint32_t dly = 5_ms){
        static uint8_t step_idx;
        if( n == 0 ) return;
        int dir = n > 0 ? 1 : -1;
        if( n < 0 ) n = -n;
        if( dly < 2_ms ) dly = 2_ms;
        while(n--){
            for(auto i = 0; i < 4; i++){
                m_pins[i].latval( m_step_table[step_idx][i] );
            }
            step_idx += dir;
            step_idx and_eq 3;
            Delay::wait( dly );
        }
        stop();
    }

    private:

    Pins (&m_pins)[4];

    const bool m_step_table[4][4]{
        {1,0,0,1}, {0,1,0,1}, {0,1,1,0}, {1,0,1,0}
    };

};

int main()
{
    //just get/store resets cause (not used here,though)
    Resets::cause();

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    info.on(true);  //uart on

    //+ turn on markup
    //! reset colors/attributes/cls/home
    info.printf("{+!Y}\r\nTesting {/M}Stepper Motor driver{|W}\r\n");

    StepperDriver s( stepper_pins );
    bool en = false;
    int steps = 400;
    uint32_t speed = 2_ms;

    auto check_sw3 = [&en](){
        if( sw3.ison() ){
            en = not en;
            while( sw3.ison() );
            Delay::wait( 50_ms ); //blocking
            info.printf("stepper: %s{W}\r\n", en ? "{G}ON" : "{R}OFF" );
        }
    };

    for(;;){
        check_sw3();
        //adc vals 0-4095 -> speed range 2_ms to 2_ms+(4095us)
        //speed = 2000us to 6095us
        speed = pot.adcval() + 2_ms;

        if( en ){
            info.printf( "step: {G}%+d{W}  speed: {G}%d us{W}\r\n", steps, speed );
            s.step( steps, speed );
            Delay d{2_sec}; //non-blocking
            while( check_sw3(), en and not d.expired() );
        }
        steps = -steps;
    }

}