#include "Irq.hpp"

namespace Irq {
    void proxtimer( IRQ_TPC pri ){
        Reg::clr((volatile uint32_t*)BASE_ADDR,7<<8);
        Reg::set((volatile uint32_t*)BASE_ADDR,pri<<8);
    }
    void set_( uint8_t v ){ Reg::set( (volatile uint32_t*)BASE_ADDR, v ); }
    void clr_( uint8_t v ){ Reg::clr( (volatile uint32_t*)BASE_ADDR, v ); }
    void eint4_rising( void ){    set_( INT4EP ); }
    void eint4_falling( void ){   clr_( INT4EP ); }
    void eint3_rising( void ){    set_( INT3EP ); }
    void eint3_falling( void ){   clr_( INT3EP ); }
    void eint2_rising( void ){    set_( INT2EP ); }
    void eint2_falling( void ){   clr_( INT2EP ); }
    void eint1_rising( void ){    set_( INT1EP ); }
    void eint1_falling( void ){   clr_( INT1EP ); }
    void eint0_rising( void ){    set_( INT0EP ); }
    void eint0_falling( void ){   clr_( INT0EP ); }
}