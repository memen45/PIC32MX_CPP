/*
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 stepper motor test

 stepper motor - 17HS13-0404S1 (stepperonline, via ebay)

 stepper controller - Toshiba TB6612FNG (sparkfun board)

 stdby, pwma, pwmb - all tied to vcc
 vm - using 9v battery
 C0 = AI1
 C2 = AI2
 B0 = BI1
 B2 = BI2

*/

#include <cstdint>

#include "Pins.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Uart.hpp"

//uart for info
Uart info{Uart::UART2, Pins::C6, Pins::C7, 1000000};

//pins
Pins sw3{ Pins::C4, Pins::INPU };   //enable/disable stepper test
Pins sw2{ Pins::C10, Pins::INPU };  //brake enable/disable
Pins sw1{ Pins::B9, Pins::INPU };   //stepper mode
Pins pot{ Pins::AN14 };             //set stepper speed

//stepper class
struct StepperDriver {

    StepperDriver( Pins::RPN ai1, Pins::RPN ai2, Pins::RPN bi1, Pins::RPN bi2,
        uint16_t steps_rev )
    : m_AI1(ai1, Pins::OUT), m_AI2(ai2, Pins::OUT),
      m_BI1(bi1, Pins::OUT), m_BI2(bi2, Pins::OUT),
      m_steps_rev(steps_rev)
    {
    }

    //stepper modes- One Phase On, Two Phase On
    enum MODE { PHASEON1, PHASEON2 };

    void mode(MODE m){ m_mode = m; }
    MODE mode() const { return m_mode; }

    //stop stepper
    void stop(void){
        //if brake on, brake w/both AI high as pwm's are tied to Vcc
        m_AI1.latval( m_brake ); m_AI2.latval( m_brake );
        m_BI1.off(); m_BI2.off();
    }

    //set/get brake enable
    void brake(bool tf){ m_brake = tf; }
    bool brake(void) const { return m_brake; }

    //step the motor
    void step(int n, uint32_t rpm = 150){
        static uint8_t step_idx;    //keep track of where we are
        if( n == 0 ) return;        //nothing to do
        int dir = n > 0 ? 1 : -1;   //which dir through step table
        if( n < 0 ) n = -n;         //make number of steps positive
        if( rpm > 150 ) rpm = 150;  //max rpm for this config
        uint32_t dly = 60000000 / (rpm * m_steps_rev); //rpm -> us delay
        while(n--){
            m_AI1.latval( m_step_table[m_mode][step_idx][0] == POS );
            m_AI2.latval( m_step_table[m_mode][step_idx][0] == NEG );
            m_BI1.latval( m_step_table[m_mode][step_idx][1] == POS);
            m_BI2.latval( m_step_table[m_mode][step_idx][1] == NEG );
            step_idx += dir;    //next step
            step_idx and_eq 3;  //stay inside table
            Delay::wait( dly ); //delay to get rpm
        }
        stop(); //done, stop will turn off all, or brake
    }

    private:

    Pins        m_AI1, m_AI2, m_BI1, m_BI2; //pins used
    uint16_t    m_steps_rev;                //steps per rev
    MODE        m_mode{PHASEON1};           //stepper mode
    bool        m_brake{false};             //brake enable

    enum COIL { OFF, POS, NEG };
    const COIL m_step_table[2][4][2]{
        { {POS,OFF}, {OFF,NEG}, {NEG,OFF}, {OFF,POS} }, //PHASEON1
        { {POS,NEG}, {NEG,NEG}, {NEG,POS}, {POS,POS} }, //PHASEON2
    };

};

int main()
{

    Resets::cause();                        //get/store resets cause

    Osc osc;                                //set osc to 24MHz
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc (fuses may also set)
    osc.tun_auto(true);                     //let sosc tune frc

    info.on(true);                          //uart on (after osc set)

    //markup->  + turn on markup
    //          ! reset colors/attributes/cls/home
    //          M magenta foreground
    //          W white foreground
    //see Markup.hpp for info
    info.printf("{+!M}\r\nStepper Motor driver test{W}\r\n");

    //set pins, motor steps per rev
    StepperDriver s( Pins::C0, Pins::C2, Pins::B0, Pins::B2, 200 );
    bool en = false;
    int steps = 400;        //2 turns each time
    uint32_t speed = 150;   //rpm (fastest can do with current config)

    //debounce a switch
    auto debounce = [](Pins& p){
        Delay d{ 50_ms };               //need 50ms of continuous sw off
        while( not d.expired() ){
            if( p.ison() ) d.restart(); //bounce/still pressed, start over
        }

    };

    //check all switches
    auto check_sw = [&](){
        if( sw3.ison() ){
            en = not en;
            debounce( sw3 );
            info.printf("%10s: %s{W}\r\n", "stepper", en ? "{G}ON" : "{R}OFF" );
        }
        if( sw2.ison() ){
            s.brake( not s.brake() );
            debounce( sw2 );
            info.printf("%10s: %s{W}\r\n", "brake", s.brake() ? "{G}ON" : "{R}OFF" );
            if( not en ) s.stop();
        }
        if( sw1.ison() ){
            s.mode( s.mode() == s.PHASEON1 ? s.PHASEON2 : s.PHASEON1  );
            debounce( sw1 );
            info.printf("%10s: {G}%s{W}\r\n", "mode",
                    s.mode() == s.PHASEON2 ? "PHASEON2" : "PHASEON1" );

        }
    };


    //2 rotations CW, 2 rotations CCW, repeat
    //sw3 for on/off, sw2 for brake on/off, sw1 for step mode
    //pot controls step speed 23-150rpm
    for(;;){
        check_sw();
        //adc vals 0-4095 -> 0-127
        //speed range 150rpm - 23rpm
        speed = (pot.adcval()>>5) + 23;
        if( en ){
            info.printf( "%10s: {G}%+d{W}  speed: {G}%d rpm{W}\r\n",
                    "step", steps, speed );
            int i = steps > 0 ? steps : -steps; //to positive count
            //do one step at a time so can check switches while stepping
            for(; en and i; i--, check_sw()){
                s.step( steps > 0 ? 1 : -1, speed );
            }
            Delay d{2_sec}; //non-blocking
            while( check_sw(), en and not d.expired() ); //check switches while waiting
        }
        steps = -steps;
    }

}