#pragma once

//Coprocessor0  Count, Compare

#include <cstdint>

struct Cp0 {

    static uint32_t     count           ();
    static uint32_t     compare         ();
    static void         count           (uint32_t);
    static void         compare         (uint32_t);
    static void         compare_reload  ();
    static void         compare_irq     (bool);
    static void         compare_us      (uint32_t);
    static void         compare_ms      (uint16_t);

};
