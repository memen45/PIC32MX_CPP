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
Pins sw3{ Pins::C4, Pins::INPU };   //enable/disable stepper test
Pins sw2{ Pins::C10, Pins::INPU };  //brake enable/disable
Pins pot{ Pins::AN14 };             //set stepper speed
Pins stepper_pins[4]{               //stepper pins
    {Pins::C0, Pins::OUT}, //AI1
    {Pins::C2, Pins::OUT}, //AI2
    {Pins::B0, Pins::OUT}, //BI1
    {Pins::B2, Pins::OUT}, //BI2
};

struct StepperDriver {

    StepperDriver( Pins (&pins)[4], uint16_t steps )
    : m_pins(pins), m_steps_rev(steps)
    {
    }

    void stop(void){
        for( auto& p : m_pins ) p.off();
    }

    void brake(void){
        stop();
        //brake - short one coil together (high in this case)
        //via both AI pins enabled (since pwm is tied high)
        m_pins[0].on();
        m_pins[1].on();
    }

    void step(int n, uint32_t rpm = 150){
        static uint8_t step_idx;
        if( n == 0 ) return;
        int dir = n > 0 ? 1 : -1;
        if( n < 0 ) n = -n;
        //if( dly < 2_ms ) dly = 2_ms;
        if( rpm > 150 ) rpm = 150;
        //dly in us calculated from rpm
        uint32_t dly = 60000000 / (rpm * m_steps_rev);
        while(n--){
            m_pins[0].latval( m_step_table[step_idx][0] ); //AI1
            m_pins[1].latval( !m_step_table[step_idx][0] ); //AI2
            m_pins[2].latval( m_step_table[step_idx][1] ); //BI1
            m_pins[3].latval( !m_step_table[step_idx][1] ); //BI2
            step_idx += dir;
            step_idx and_eq 3;
            Delay::wait( dly );
        }
        stop();
    }

    private:

    Pins (&m_pins)[4]; //AI1, AI2, BI1,BI2
    uint16_t m_steps_rev; //steps per rev
    const bool m_step_table[4][2]{
        //AI1 (AI2=!AI1), BI1 (BI2=!BI1)
        {1,0}, {0,0}, {0,1}, {1,1}
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

    StepperDriver s( stepper_pins, 200 );
    bool en = false, brake = false;;
    int steps = 400; //2 turns
    uint32_t speed = 150; //rpm (fastest can do in current config)

    auto check_sw = [&](){
        if( sw3.ison() ){
            en = not en;
            while( sw3.ison() ); //wait for release
            Delay::wait( 50_ms ); //blocking (debounce)
            info.printf("stepper: %s{W}\r\n", en ? "{G}ON" : "{R}OFF" );
        }
        if( sw2.ison() ){
            brake = not brake;
            while( sw2.ison() ); //wait for release
            Delay::wait( 50_ms ); //blocking (debounce)
            info.printf("brake: %s{W}\r\n", brake ? "{G}ON" : "{R}OFF" );
            if( not en ) { if( brake ) s.brake(); else s.stop(); }
        }
    };


    //2 rotations CW, 2 rotations CCW, repeat
    //sw3 for on/off, sw2 for brake on/off
    //pot controls step speed 2000us-6095us
    for(;;){
        check_sw();
        //adc vals 0-4095 -> 0-127
        //speed range 150rpm - 23rpm
        speed = (pot.adcval()>>5) + 23;
        if( en ){
            info.printf( "step: {G}%+d{W}  speed: {G}%d rpm{W}\r\n", steps, speed );
            int i = steps > 0 ? steps : -steps;
            for(; en and i; i--, check_sw()){
                s.step( steps > 0 ? 1 : -1, speed );
            }
            Delay d{2_sec}; //non-blocking
            while( check_sw(), en and not d.expired() ); //check switches while waiting
            if( brake ) s.brake();
        }
        steps = -steps;
    }

}