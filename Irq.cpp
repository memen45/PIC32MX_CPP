#include "Irq.hpp"

namespace Irq {
    void clear_intconbit( uint8_t bit ){
        *(uint32_t*)(BASE_ADDR+1) = (1<<bit); //CLR
    }
    void set_intconbit( uint8_t bit ){
        *(uint32_t*)(BASE_ADDR+2) = (1<<bit); //SET
    }
    void proxtimer( IRQ_TPC pri ){
        *(uint32_t*)(BASE_ADDR+1) = (7<<8);
        *(uint32_t*)(BASE_ADDR+2) = pri<<8;
    }
    void eint4_rising(){    set_intconbit( INT4EP ); }
    void eint4_falling(){   clear_intconbit( INT4EP ); }
    void eint3_rising(){    set_intconbit( INT3EP ); }
    void eint3_falling(){   clear_intconbit( INT3EP ); }
    void eint2_rising(){    set_intconbit( INT2EP ); }
    void eint2_falling(){   clear_intconbit( INT2EP ); }
    void eint1_rising(){    set_intconbit( INT1EP ); }
    void eint1_falling(){   clear_intconbit( INT1EP ); }
    void eint0_rising(){    set_intconbit( INT0EP ); }
    void eint0_falling(){   clear_intconbit( INT0EP ); }
}