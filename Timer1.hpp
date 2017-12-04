#ifndef _TIMER1_H
#define _TIMER1_H

#include <cstdint>

namespace Timer1 {

    enum {
        SET = 2, CLR = 1,
        T1CON = 0xBF808000,
        T1CONSET = 0xBF808008,
        T1CONCLR = 0xBF808004,
        TMR1 = 0xBF808010,
        PR1 = 0xBF808020,
        ON = 1<<15, SIDL = 1<<13, TWDIS = 1<<12, TWIP = 1<<11,
        EXT_RES = 3<<8, EXT_LPRC = 2<<8, EXT_T1CK = 1<<8, EXT_SOSC = 0<<8,
        TGATE = 1<<7,
        TCKPS_256 = 3<<4, TCKPS_64 = 2<<4, TCKPS_8 = 1<<4, TCKPS_1 = 0<<4,
        TSYNC = 1<<2,
        TCS = 1<<1
    };

    void timer1( uint16_t n ){ *(volatile uint32_t*)TMR1 = n; }
    uint16_t timer1( void ){ return *(volatile uint32_t*)TMR1; }

    void pr1( uint16_t n ){ *(volatile uint32_t*)PR1 = n; }
    uint16_t pr1( void ){ return *(volatile uint32_t*)PR1; }

    void on( void ){    *(volatile uint32_t*)(T1CONSET) = ON; }
    void off( void ){   *(volatile uint32_t*)(T1CONCLR) = ON; }

    void stop_idle( void ){ *(volatile uint32_t*)(T1CONSET) = SIDL; }
    void cont_idle( void ){ *(volatile uint32_t*)(T1CONCLR) = SIDL; }

    void async_wrdis( void ){ *(volatile uint32_t*)(T1CONSET) = TWDIS; }
    void async_wren( void ){ *(volatile uint32_t*)(T1CONCLR) = TWDIS; }

    bool wr_busy( void ){ return *(volatile uint32_t*)(T1CON) & TWIP; }

    void clk_sosc( void ){
        *(volatile uint32_t*)(T1CON+CLR) = EXT_RES;
    }
    void clk_lprc( void ){
        clk_sosc();
        *(volatile uint32_t*)(T1CON+SET) = EXT_LPRC;
    }
    void clk_t1ck( void ){
        clk_sosc();
        *(volatile uint32_t*)(T1CON+SET) = EXT_T1CK;
    }

    void tgate_on( void ){      *(volatile uint32_t*)(T1CONSET) = TGATE; }
    void tgate_off( void ){     *(volatile uint32_t*)(T1CONCLR) = TGATE; }

    void prescale_1( void ){    *(volatile uint32_t*)(T1CONCLR) = TCKPS_256; }
    void prescale_256( void ){  *(volatile uint32_t*)(T1CONSET) = TCKPS_256; }
    void prescale_64( void ){
        prescale_1();
        *(volatile uint32_t*)(T1CONSET) = TCKPS_64;
    }
    void prescale_8( void ){
        prescale_1();
        *(volatile uint32_t*)(T1CONSET) = TCKPS_8;
    }

    void tsync_on( void ){      *(volatile uint32_t*)(T1CONSET) = TSYNC; }
    void tsync_off( void ){     *(volatile uint32_t*)(T1CONCLR) = TSYNC; }

    void ext_clk( void ){       *(volatile uint32_t*)(T1CONSET) = TCS; }
    void int_clk( void ){       *(volatile uint32_t*)(T1CONCLR) = TCS; }

}

#endif //_TIMER1_H
