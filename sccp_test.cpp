#include "Pins.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Ccp.hpp"

//testing SCCP4

//jumper B8 to C13 (led2) so we can light up led on a pps out pin
//curiosity board has no leds on a pps pin

int main()
{
    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    Sccp sccp4{ Sccp::CCP4 };
    sccp4.mode(sccp4.DEPWM16);              //dual edge pwm 16bit
    sccp4.compa(0);
    sccp4.compb(0);
    sccp4.out_pin(Pins::RP13);
    sccp4.on(true);


    for(uint16_t i = 0; ; i++, i and_eq 4095){
        sccp4.compb( i );
        Delay::wait(1_ms);
    }

}