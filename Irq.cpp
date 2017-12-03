#include "Irq.hpp"

namespace Irq {
    void proxtimer( IRQ_TPC pri ){
        *(uint32_t*)(BASE_ADDR+1) = (7<<8);
        *(uint32_t*)(BASE_ADDR+2) = pri<<8;
    }
    void eint4_rising(){    *(volatile uint32_t*)(BASE_ADDR+2) = (1<<INT4EP ); }
    void eint4_falling(){   *(volatile uint32_t*)(BASE_ADDR+1) = (1<<INT4EP ); }
    void eint3_rising(){    *(volatile uint32_t*)(BASE_ADDR+2) = (1<<INT3EP ); }
    void eint3_falling(){   *(volatile uint32_t*)(BASE_ADDR+1) = (1<<INT3EP ); }
    void eint2_rising(){    *(volatile uint32_t*)(BASE_ADDR+2) = (1<<INT2EP ); }
    void eint2_falling(){   *(volatile uint32_t*)(BASE_ADDR+1) = (1<<INT2EP ); }
    void eint1_rising(){    *(volatile uint32_t*)(BASE_ADDR+2) = (1<<INT1EP ); }
    void eint1_falling(){   *(volatile uint32_t*)(BASE_ADDR+1) = (1<<INT1EP ); }
    void eint0_rising(){    *(volatile uint32_t*)(BASE_ADDR+2) = (1<<INT0EP ); }
    void eint0_falling(){   *(volatile uint32_t*)(BASE_ADDR+1) = (1<<INT0EP ); }
}