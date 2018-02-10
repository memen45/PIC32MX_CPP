#include "Vin.hpp"
#include "Adc.hpp"

//=============================================================================
                    Vin::Vin            (Pins::RPN pp)
//=============================================================================
    : Pins(pp, Pins::AIN)
{
};

//=============================================================================
    uint16_t        Vin::adcval         ()
//=============================================================================
{
    Adc adc;
    adc.mode_12bit(true);           //12bit mode
    adc.trig_sel(adc.AUTO);         //adc starts conversion
    adc.samp_time(31);              //max sampling time- 31Tad
    adc.conv_time(adc.PBCLK12BIT);  //if no arg,default is 4 (for 24MHz)
    adc.ch_sel(an_num());           //ANn (AVss if no ANn for pin)
    adc.on(true);
    adc.samp(true);
    while(not Adc::done());         //blocking
    return Adc::read();             //buf[0]
}

