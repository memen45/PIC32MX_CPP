#pragma once

/*=============================================================================
 Coprocessor0  Count, Compare
=============================================================================*/

#include <cstdint>
#include "Irq.hpp"
#include "Reg.hpp"
#include "Osc.hpp"

struct Cp0 {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    static uint32_t     count           ();
    static uint32_t     compare         ();
    static void         count           (uint32_t);
    static void         compare         (uint32_t);
    static void         compare_reload  (bool = false);
    static void         compare_us      (uint32_t);
    static void         compare_ms      (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Irq ir;
    static Osc osc;
};

/*=============================================================================
 all functions inline
=============================================================================*/

//vars
static uint32_t m_compare_count; //save count for reloads

uint32_t Cp0::count(){
    return __builtin_mfc0(9,0);
} //get
uint32_t Cp0::compare(){
    return __builtin_mfc0(11,0);
} //get
void Cp0::count(uint32_t v){
    __builtin_mtc0(9,0,v);
} //set
void Cp0::compare(uint32_t v){
    __builtin_mtc0(11,0,v);
} //set

//true = clear flag and enable cp0 irq
void Cp0::compare_reload(bool tf){
    compare(count() + m_compare_count);
    if(tf){
        ir.flag_clr(ir.CORE_TIMER);
        ir.on(ir.CORE_TIMER, true);
    }
}
void Cp0::compare_us(uint32_t v){
    m_compare_count = osc.speed() / 2000000 * v; //cpu speed from Osc::
    compare_reload();
}
void Cp0::compare_ms(uint16_t v){
    compare_us(v * 1000);
}
