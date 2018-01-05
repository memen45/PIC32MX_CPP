/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 testing - using C++

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/



/*=============================================================================
 Includes
=============================================================================*/
#include <cstdint>

#include "Delay.hpp"
#include "Pins.hpp"
#include "Irq.hpp"
#include "Cp0.hpp"
#include "Timer1.hpp"
#include "Timer23.hpp"
#include "Pmd.hpp"
#include "Wdt.hpp"
#include "Rtcc.hpp"
#include "Comp123.hpp"
#include "Clc.hpp"
#include "Adc.hpp"
#include "Resets.hpp"
#include "Usb.hpp"
#include "Osc.hpp"


/*=============================================================================
 LED's - all in array
 Pin class does not init pin, so we can control exactly if/when we want to
 set them up
 array is created here so we can init them all at once in a loop, and access
 them later by index
=============================================================================*/
Pins leds[] = {                 //group leds
    { Pins::D1 },               //RED
    { Pins::C3 },               //GREEN
    { Pins::C15 },              //BLUE
    { Pins::D3 },               //LED1 (invert in timer1/timer2/timer3 irq)
    { Pins::C13 }               //LED2 (cp0 irq blinks)
};
Pins& led1 = leds[3];           //references to specific leds as needed
Pins& led2 = leds[4];

/*=============================================================================
 Switches - all in array
=============================================================================*/
Pins sw[] = {                   //true=lowison
    { Pins::B9,  true },        //SW1 (rotate delays))
    { Pins::C10, true },        //SW2 cp0 irq blink rate++
    { Pins::C4,  true }         //SW3 cp0 irq blink rate--
};
Pins& sw1 = sw[0];              //sw1/2/3 references
Pins& sw2 = sw[1];
Pins& sw3 = sw[2];

/*=============================================================================
 Delays
=============================================================================*/
DelayCP0 sw_dly;                //debounce
DelayCP0 dly[3];                //CP0 led delays
uint32_t t_ms[] = {             //led delay ms
    200, 400, 600,
    1000, 2000, 4000,
    4000, 8000, 1600,
    0
};

/*=============================================================================
 Timers
=============================================================================*/
Timer23 timer2(Timer23::T2);
Timer23 timer3(Timer23::T3);

/*=============================================================================
 Irq's
 (letting USB code take care of setting up/enabling USB irq)
=============================================================================*/
Irq::irq_list_t irqlist[] = {               //list of irq's to init/enable
    //vector#           pri sub enable
    { Irq::TIMER_1,     1,  0,  true },
    { Irq::TIMER_2,     1,  0,  true },
    { Irq::TIMER_3,     1,  0,  true },
    { Irq::CORE_TIMER,  5,  0,  true },
    { Irq::RTCC,        1,  0,  true },
    { Irq::END }                            //need END or else bad happens
};

/*=============================================================================
 Modules disable
=============================================================================*/
Pmd::PMD pmd_list[] = {                      //list of modules to disable
    Pmd::USB,
    Pmd::HLVD, Pmd::CMP1, Pmd::CMP2, Pmd::CMP3,
    Pmd::I2C1, Pmd::I2C2, Pmd::I2C3,
    Pmd::T1, Pmd::T2, Pmd::T3,
    Pmd::END                                //need END or else bad happens
};

/*=============================================================================
 MAIN
=============================================================================*/
int main(){

    //pic32mm curiosity board, Fluke 101 (not TrueRMS), test
    //    DelayCP0 tmr;
    //    Pins rc0(Pins::C0); Pins rc2(Pins::C2);
    //    rc0.digital_out(); rc2.digital_out();
    //    rc2.high(); rc0.low();
    //    for(;;rc0.invert(),rc2.invert(),tmr.wait_us(8333) );
    // 3.64vac 60hz
    //for(;;rc0.invert(),rc2.invert(),tmr.wait_ms(5000) );
    // +3.29vdc , -3.29vdc

    //__________________________________________________________________________
    Resets::CAUSE cause = Resets::cause();  //use cause result somewhere
                                            //(will be EXTR mostly with pkob)

    Osc osc;
    osc.pllset(osc.MUL12, osc.DIV4);        // 24mhz - 96MHz for usb (/2)

    //__________________________________________________________________________
    //usb- can't do much
    //(no Osc.hpp yet, so pll output running at 24MHz, usb clock is then 12MHz)
    //just run something usb to prevent compiler from optimizing away
    //usb code, so can see if have (bad) code (compile errors)
    UsbHandlers::init();

    //__________________________________________________________________________
    //try adc - pot on curiosity board
    Adc adc;
    adc.mode_12bit(true);                   //12bit mode
    adc.trig_sel(adc.AUTO);                 //adc starts conversion
    adc.samp_time(31);                      //max sampling time- 31Tad
    adc.conv_time(adc.PBCLK12BIT);          //if no arg,default is 4 (for 24MHz)
    adc.ch_sel(adc.AN14);                   //pot- RC8/AN14 (default ANSEL/TRIS)
    adc.on(true);
    //to get adc (polling, non-blocking)
    //Adc::samp(true);    //start sample, auto conversion
    //if(Adc::done()){    //check if done
    //  res = Adc::buf(0);//get result
    //  Adc::samp(true);  //and restart (done bit cleared when samp=1)
    //}

    //__________________________________________________________________________
    //try some compare functions (does nothing)
    Comp123 c1(Comp123::C1);
    Comp123 c2(Comp123::C2);
    c1.on(false);
    c2.on(false);
    Comp123::stop_idle(true); //<--use Comp123:: for common function
    Comp123::cref_sel(Comp123::INT_BGAP);

    //__________________________________________________________________________
    //try clc (does nothing)
    Clc clc1(Clc::CLC1);
    clc1.in_sel(1, 3);
    clc1.in_sel(4, 6);
    clc1.gate_sel(0x12345678);
    clc1.mode(clc1.ORXOR);

    //__________________________________________________________________________
    //test rtcc (somewhat)
    Rtcc rtcc;
    rtcc.clk_sel(rtcc.SOSC);                //external sosc (curiosity board)
                                            //(soscen automatically set by rtcc)
    rtcc.clk_div(rtcc.CLK_DIV_32KHZ);       //already calculated div for lprc
    rtcc.alarm_interval(rtcc.MINUTE1);      //alarm every minute
    rtcc.chime(true);                       //repeating alarm
    rtcc.alarm(true);                       //turn on alarm

    const uint32_t test_time = (2<<28 | 3<<24 | 4<<20 | 5<<16 | 0<<12 | 1<<8);
    rtcc.time(test_time);                   //23:45:01
//    if(rtcc.time() != test_time){           //did write work? (testing)
//        while(1);                           //lockup if read/write not working
//    }                                       //else syskey/wrlock working
    rtcc.on(true);                          //turn on, alarm is 00:00:00
                                            //(alarm every 00 seconds match)
                                            //disable/enable core timer irq
                                            //every minute in rtcc isr

    //__________________________________________________________________________
    //peripheral module disable
    Pmd::off(pmd_list);                     //test Pmd disable (T1/2/3 disabled)
                                            //can verify T1,T2,T3 no longer work
                                            //with below enable commented out

    Pmd::on(pmd_list);                      //test Pmd enable- all back on again

    //__________________________________________________________________________
    //see if pps code works (does nothing)
    //(do before pins setup below, as out will clear tris)
    //use RAx or RPx for pins (has to be the 'R' versions)
    Pins::pps_in(Pins::U2RX, Pins::RP1); //RP1(RA0) alsp set to input
    Pins::pps_in(Pins::U2RX); //default is Pins::RPNONE (peripheral input off)
    Pins::pps_out(Pins::U2TX, Pins::RA0); //TX2->RA0(RP1)
    Pins::pps_out(Pins::PPSLAT, Pins::RA0); //RA0 now uses LATx

    //__________________________________________________________________________
    //init sw pins
    for(auto& s : sw){
        s.digital_in();
        s.pullup(true);
    }

    //__________________________________________________________________________
    //init leds
    for(auto& l : leds){
        l.digital_out();
        l.on();                             //show each led working
        auto d = cause == Resets::EXTR ? 500 : 2000; //ext- 1/2sec, else 2sec
        sw_dly.wait_ms(d); //wait
        l.off();                            //then off
    }

    //__________________________________________________________________________
    //init timers
    Timer1::prescale(Timer1::PS256);        //prescale 1:256
    Timer1::on(true);                       //turn on timer1
    timer2.prescale(Timer23::PS64);         //prescale 1:64
    timer2.on(true);                        //turn on timer2
    timer3.prescale(Timer23::PS32);         //prescale 1:32
    timer3.on(true);                        //turn on timer3

    //__________________________________________________________________________
    //cp0 compare timeout
    Cp0::compare_ms(200);


    //__________________________________________________________________________
    //nested function
    auto rotate_delays = [ & ](){           //init delays or rotate delays
        static uint8_t start = 0;
        for(auto& d : dly){
            if(! t_ms[start]) start = 0;
            d.set_ms(t_ms[start++]);
        }
    };
    rotate_delays();                        //initialize delays

    //__________________________________________________________________________
    //nested function
    auto irq_blinkrate = [ & ](int16_t n){  //sw1/2 adjust cp0 irq rate +/-
        static uint16_t rate = 200;
        rate += n;
        if(rate < 100 || rate > 4000) rate -= n;
        Cp0::compare_ms(rate);              //no need to disable irq's
        Cp0::compare_reload(true);          //true = clear flag, cp0 irq on
        led2.on();                          //as cp0 irq only reloads (reads)
        Irq::enable_all();
    };

    //__________________________________________________________________________
    //irq's init/enable
    Irq::init(irqlist);                     //init all irq's
    Irq::shadow_set(5, 1);                  //priority5 (usb) using shadow set
    Irq::enable_all();                      //global irq enable

    //__________________________________________________________________________
    //start first adc sample on AN14
    Adc::samp(true);

    //__________________________________________________________________________
    //watchdog timer
    Wdt::on(true);                          //try the watchdog
                                            //RWDTPS = PS8192, so 8ms timeout


    //<><><><><><><><><><><><><><><<><><><><><><><><><><><><><><><><><><><><><><
    //here we go
    for (;;){
        Wdt::reset();                       //comment out to test wdt reset

        //check for delay timeouts, invert led, reset delay counter
        for(auto i = 0; i < 3; i++){
            if(! dly[i].expired()) continue;
            leds[i].invert();
            dly[i].reset();
        }
        //check adc - swreset if < 10,
        //disable led1 if > 1/2Vref+(Vdd)
        //(by setting led1 to digital in)
        if(Adc::done()){
            auto r = Adc::bufn(0);
            if(r < 10) Resets::swreset();
            if(r > (4096/2)) led1.digital_in();
            else led1.digital_out();
            Adc::samp(true); //start again
        }
        //check sw1, rotate delay times among the 3 color led's
        if(sw1.ison()){
            if(sw_dly.expired()){           //is a new press
                rotate_delays();            //do something visible
            }
            sw_dly.set_ms(100);             //sw debounce (use for all switches)
            //sw_dly sets while pressed
            //so will require sw release of 100ms
            //before sw_dly timer can expire
        }
        //check sw2, dec blink rate of led2
        if(sw2.ison()){
            if(sw_dly.expired()){
                irq_blinkrate(100);         //++
            }
            sw_dly.set_ms(100);             //sw debounce (use for all switches)
        }
        //check sw3, inc blink rate of led2
        if(sw3.ison()){
            if(sw_dly.expired()){
                irq_blinkrate(-100);        //--
            }
            sw_dly.set_ms(100);             //sw debounce (use for all switches)
        }
    }

}

/*=============================================================================
 Interrupt code
 irq's need to be init in other code for the priority (Irq::init)
 and if using shadow set (Irq::shadow_set)
=============================================================================*/
ISR(CORE_TIMER){
    Irq ir; Cp0 cp; //can use IrQ::func() syntax also
    cp.compare_reload();
    led2.invert();
    ir.flag_clr(ir.CORE_TIMER);
}

ISR(TIMER_1){
    Irq ir;
    led1.invert();
    ir.flag_clr(ir.TIMER_1);
}
ISR(TIMER_2){
    Irq ir;
    led1.invert();
    ir.flag_clr(ir.TIMER_2);
}
ISR(TIMER_3){
    Irq ir;
    led1.invert();
    ir.flag_clr(ir.TIMER_3);
}
ISR(RTCC){
    Irq ir; Cp0 cp;
    static bool b;
    if(b = !b){ //toggle and check
        ir.on(ir.CORE_TIMER, false); //core timer irq disable
        led2.off();
    } else {
        cp.compare_reload(true); //true = clear flag, core timer irq on
    }
    ir.flag_clr(ir.RTCC);
}
