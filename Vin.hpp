#pragma once

#include "Pins.hpp"

//simple Vin class to measure voltage at pin input via adc
//adc read is blocking
//if pin has no ANn, will use AVss as adc channel (should return 0)

struct Vin : protected Pins {

                Vin         (Pins::RPN);
    uint16_t    adcval      ();

};
