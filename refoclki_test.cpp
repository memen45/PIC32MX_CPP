#include "Pins.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"

int main()
{
    //just get/store resets cause (not used here,though)
    Resets::cause();

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    //testing REFOCLKI using a pin for a clock (manually toggle)
    Pins led1{Pins::D3, Pins::OUT};
    Pins clki{Pins::D0, Pins::OUT};         //drive refclki
    Pins clko{Pins::B15, Pins::IN};         //watch refclko
    osc.refo_on(osc.RREFCLKI);              //pin34 RD0 in
    osc.refo_div(250);                      //divide by 250
    osc.refo_out(true);                     //pin3 RB15 out

    //loop runs every 1ms, 500Hz clock
    //should blink at 1Hz (500Hz /2/250 = 1Hz)
    for(;;){
        clki.invert();
        if(clko.pinval()) led1.on(); else led1.off();
        Delay::wait(1_ms);
    }

}