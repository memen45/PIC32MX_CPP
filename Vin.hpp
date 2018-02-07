#pragma once

#include "Reg.hpp"
#include "Pins.hpp"
#include "Adc.hpp"

//simple Vin class to measure voltage at pin input via adc

//adc read is blocking
//if pin has no ANn, will use AVss as adc channel (should return 0)
struct Vin : protected Pins {

    Vin(Pins::RPN);

    uint16_t adcval(){
        Adc adc;
        adc.mode_12bit(true);           //12bit mode
        adc.trig_sel(adc.AUTO);         //adc starts conversion
        adc.samp_time(31);              //max sampling time- 31Tad
        adc.conv_time(adc.PBCLK12BIT);  //if no arg,default is 4 (for 24MHz)
        adc.ch_sel(an_num());           //ANn (AVss if no ANn for pin)
        adc.on(true);
        adc.samp(true);
        while(not Adc::done());         //blocking
        return Adc::read(0);            //buf[0]
    }
};

Vin::Vin(Pins::RPN pp)
    : Pins(pp, Pins::AIN)
{
};