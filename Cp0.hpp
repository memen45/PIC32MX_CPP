#pragma once

//Coprocessor0  Count, Compare
//32bit Count in CP0 increments at Fosc/2

#include <cstdint>

struct Cp0 {

            //get cp0 count
            static auto
count       () -> uint32_t;

            //get cp0 compare
            static auto
compare     () -> uint32_t;

            //set cp0 count
            static auto
count       (uint32_t) -> void;

            //set cp0 compare
            static auto
compare     (uint32_t) -> void;

            //add to count the calculated value previously set with
            //compare_us/ms, and store into compare
            //(can set compare_us/ms, enable irq, in irq just use compare_reload())
            static auto
compare_reload () -> void;

            //enable/disable the core timer irq
            static auto
compare_irq (bool) -> void;

            //calculate/store the value into compare
            //so that a count/compare match occurs in n us
            static auto
compare_us  (uint32_t) -> void;

            //calculate/store the value into compare
            //so that a count/compare match occurs in n ms
            static auto
compare_ms  (uint16_t) -> void;

};
