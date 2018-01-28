/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 testing - using C++

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/

#include <cstdint>




//try different apps
#define MAIN 1 //simple blink 2 leds, rgb led's
//#define MAIN 2 //most peripherals touched


//=============================================================================
#if MAIN == 1

#include <cstdint>
#include "Pins.hpp"
#include "Wdt.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Ccp.hpp"

//set led pins
Pins led1(Pins::D3);
Pins led2(Pins::C13);
Pins ledR(Pins::D1);
Pins ledG(Pins::C3);
Pins ledB(Pins::C15);

//pwm to rgb pins
//mccp 1-3 pwm to rgb led's
Ccp rgb[] = {
    Ccp::CCP1,
    Ccp::CCP2,
    Ccp::CCP3
};

int main()
{
    //just get/store resets cause (not used here,though)
    Resets::cause();

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    //init pins
    led1.digital_out();
    led2.digital_out();

    //init pwm
    for(auto& i : rgb){
        i.mode(i.DEPWM16);
        i.compa(0);
        i.out_pins(i.OCB);
        i.on(true);
    }
    //R,G use OCxB, B uses OCxE
    rgb[2].out_pins(rgb[2].OCE);
    //stagger pwm
    rgb[0].compb(100);
    rgb[1].compb(0x5555);
    rgb[2].compb(0xAAAA);

    //delays, polling
    Delay dly_led1, dly_led2, dly_rgb;
    dly_led1.set_ms(333);
    dly_led2.set_ms(333);
    dly_rgb.set_ms(2);


    auto rgb_do = [](){
        static bool c[] = { 1, 1, 1 };
        for(uint8_t i = 0; i < 3; i++){
            uint16_t v = rgb[i].compb();
            if(c[i]) v += 20 + i*3; else v -= 15 - i*3;
            if(v < 50){ v = 50; c[i] = 1; }
            if(v > 30000){ v = 30000; c[i] = 0; }
            rgb[i].compb(v);
        }
    };


    //loop, clear wdt (configs bits may be set to always on)
    //(start led1 in opposite state of led2)
    led1.invert();
    for(; ;Wdt::reset()){
        if(dly_led1.expired()){
            dly_led1.restart();
            led1.invert();
        }
        if(dly_led2.expired()){
            dly_led2.restart();
            led2.invert();
        }
        if(dly_rgb.expired()){
            dly_rgb.restart();
            rgb_do();
        }
    }
}



#endif
//=============================================================================




#if MAIN == 2
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
#include "Comp.hpp"
#include "Clc.hpp"
#include "Adc.hpp"
#include "Resets.hpp"
#include "Usb.hpp"
#include "Osc.hpp"
#include "Dma.hpp"
#include "Uart.hpp"
#include "Spi.hpp"
#include "I2c.hpp"
#include "Ccp.hpp"
#include "Nvm.hpp"


/*=============================================================================
 rgb LED's (array test)
=============================================================================*/
Pins leds[] = {                 //group leds
    { Pins::D1 },               //RED   //OCM1B
    { Pins::C3 },               //GREEN //OCM2B
    { Pins::C15 },              //BLUE  //OCM3E
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
Delay sw_dly;                   //debounce
Delay dly[3];                   //CP0 led delays
uint32_t t_ms[] = {             //led delay ms
    50, 60, 70,
    300, 320, 340,
    0
};

/*=============================================================================
 Timers
=============================================================================*/
Timer23 timer2(Timer23::TMR2);
Timer23 timer3(Timer23::TMR3);

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
    //    Delay tmr;
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
    //__________________________________________________________________________
    //set osc to 24MHz, use values to also get usb working
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       // 24mhz - 96MHz for usb (/2)
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    //uint32_t t = osc.extclk();              //calculate sysclock (testing)

    //__________________________________________________________________________
    //uart
    Uart u1(Uart::UART1);
    u1.on(false);
    u1.rx_addr(55);
    u1.rx_mask(255);

    //__________________________________________________________________________
    //ccp
    //mccp 1-3 pwm to rgb led's
    Ccp ccps[] = {
        Ccp::CCP1,
        Ccp::CCP2,
        Ccp::CCP3
    };
    for(auto& i : ccps){
        i.mode(i.DEPWM16);
        i.compa(0);
        i.out_pins(i.OCB);
        i.on(true);
    }
    //R,G use OCxB, B uses OCxE
    ccps[2].out_pins(ccps[2].OCE);
    ccps[0].compb(0);
    ccps[0].compb(0x2000);
    ccps[0].compb(0x4000);

    //__________________________________________________________________________
    //nvm
    Nvm nvm;
    uint8_t onerow[Nvm::ROWSIZE];
    for(uint16_t i = 0; i<Nvm::ROWSIZE; i++) onerow[i] = i;
    nvm.page_erase(Nvm::MAXROW256*Nvm::PAGESIZE);
    nvm.write_row((uint32_t*)onerow, Nvm::MAXROW256*Nvm::ROWSIZE);

    uint32_t ip = (Nvm::MAXROW256*Nvm::ROWSIZE)|Nvm::BASEMEM;
    for(uint16_t i = 0; i<Nvm::ROWSIZE; i++){
        volatile uint8_t x = Reg::val8(ip++);
        x = x;
    }

    //__________________________________________________________________________
    //sys


    //__________________________________________________________________________
    //i2c
    I2c i2c1(I2c::I2C1);
    i2c1.on(false);
    i2c1.speed(I2c::KHZ400);

    //__________________________________________________________________________
    //dma
    Dma dma0(Dma::DMA0);
    dma0.irq_abort(Irq::CORE_TIMER);
    dma0.irq_abort(Dma::IRQOFF);
    dma0.ssize(0x3355);

    //__________________________________________________________________________
    //usb- can't do much
    //(no Osc.hpp yet, so pll output running at 24MHz, usb clock is then 12MHz)
    //just run something usb to prevent compiler from optimizing away
    //usb code, so can see if have (bad) code (compile errors)
    UsbHandlers::init();


    //__________________________________________________________________________
    //SPI
    Spi spi1(Spi::SPI1);
    Spi spi2(Spi::SPI2);
    Spi spi3(Spi::SPI3);
    spi1.on(false);
    spi2.on(false);
    spi3.on(false);

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
    Comp c1(Comp::CMP1);
    Comp c2(Comp::CMP2);
    c1.on(false);
    c2.on(false);
    Comp::stop_idle(true); //<--use Comp:: for common function
    Comp::cref_sel(Comp::INT_BGAP);

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
    rtcc.alarm_interval(rtcc.MINUTE1);      //alarm every minute
    rtcc.chime(true);                       //repeating alarm
    rtcc.alarm(true);                       //turn on alarm

    const uint32_t test_time = (2<<28 | 3<<24 | 4<<20 | 5<<16 | 0<<12 | 1<<8);
    rtcc.time(test_time);                   //23:45:01
    rtcc.on(true);                          //turn on, alarm is 00:00:00
                                            //(alarm every 00 seconds match)
                                            //disable/enable core timer irq
                                            //every minute in rtcc isr
                                            //clk is lprc unless sosc is on,
                                            //div set for lprc/sosc

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
    Pins u2rx(Pins::RP1);
    u2rx.pps_in(u2rx.U2RX);
    u2rx.pps_in(u2rx.PPSINOFF);



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
            if(not t_ms[start]) start = 0;
            d.set_ms(t_ms[start++]);
        }
    };
    rotate_delays();                        //initialize delays

    //__________________________________________________________________________
    //nested function
    auto irq_blinkrate = [ & ](int16_t n){  //sw1/2 adjust cp0 irq rate +/-
        static uint16_t rate = 200;
        rate += n;
        if(rate < 100 or rate > 4000) rate -= n;
        Cp0::compare_ms(rate);
        Cp0::compare_reload();
        Cp0::compare_irq(true);             //true = clear flag, cp0 irq on
        led2.on();
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

        //check for delay timeouts, change pwm on rgb, reset delay counter
        static int updown[] = { 1, 1, 1 };
        for(auto i = 0; i < 3; i++){
            if(not dly[i].expired()) continue;
            //leds[i].invert();
            uint16_t tmp = ccps[i].compb();
            if(updown[i] == 1) tmp += 500;
            else tmp -= 500;

            if(tmp > 30000){
                updown[i] = -1;
                tmp = 30000;
            }
            if(tmp < 1000){
                updown[i] = 1;
                tmp = 1000;
            }
            ccps[i].compb(tmp);

            dly[i].restart();
        }
        //check adc - swreset if < 10,
        //disable led1 if > 1/2Vref+(Vdd)
        //(by setting led1 to digital in)
        if(Adc::done()){
            auto r = Adc::read(0);
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
    b xor_eq 1;
    if(b){
        ir.on(ir.CORE_TIMER, false); //core timer irq disable
        led2.off();
    } else {
        cp.compare_reload();
        cp.compare_irq(true);
    }
    ir.flag_clr(ir.RTCC);
}
//=============================================================================
#endif