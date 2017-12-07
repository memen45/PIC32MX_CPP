#ifndef _IRQ_H
#define _IRQ_H

/*=============================================================================
 Interrupt functions
=============================================================================*/

#include <cstdint>
#include"Reg.hpp"

namespace Irq {

    enum {
        BASE_ADDR =     0xBF80F000UL,
        BASE_ADDR_IFS = 0xBF80F040UL,
        BASE_ADDR_IEC = 0xBF80F0C0UL,
        BASE_ADDR_IPC = 0xBF80F140UL,
        INT0EP = 1<<0, INT1EP = 1<<1, INT2EP = 1<<2,
        INT3EP = 1<<3, INT4EP = 1<<4
    };

    //irq vector numbers
    enum IRQ_VN : uint8_t {
        CORE_TIMER = 0, CORE_SOFTWARE_0, CORE_SOFTWARE_1,
        EXTERNAL_0 = 3, EXTERNAL_1, EXTERNAL_2, EXTERNAL_3, EXTERNAL_4,
        CHANGE_NOTICE_A = 8, CHANGE_NOTICE_B, CHANGE_NOTICE_C, CHANGE_NOTICE_D,
        //12,13,14,15,16
        TIMER_1 = 17, TIMER_2, TIMER_3,
        //20,21,23
        COMPARATOR_1 = 23, COMPARATOR_2, COMPARATOR_3,
        //26,27,28
        USB = 29,
        RTCC = 32,
        ADC = 33,
        HLVD = 36,
        CLC1 = 37, CLC2, CLC3, CLC4,
        SPI1_ERR = 41, SPI1_TX, SPI1_RX,
        SPI2_ERR = 44, SPI2_TX, SPI2_RX,
        SPI3_ERR = 47, SPI3_TX, SPI3_RX,
        //50,51,52
        UART1_RX = 53, UART1_TX, UART1_ERR,
        UART2_RX = 56, UART2_TX, UART2_ERR,
        UART3_RX = 59, UART3_TX, UART3_ERR,
        //62,63,64
        I2C1_SLAVE = 65, I2C1_MASTER, I2C1_BUS,
        I2C2_SLAVE = 68, I2C2_MASTER, I2C2_BUS,
        I2C3_SLAVE = 71, I2C3_MASTER, I2C3_BUS,
        CCP1 = 74, CCT1,
        CCP2 = 76, CCT2,
        CCP3 = 78, CCT3,
        CCP4 = 80, CCT4,
        CCP5 = 82, CCT5,
        CCP6 = 84, CCT6,
        CCP7 = 86, CCT7,
        CCP8 = 88, CCT8,
        CCP9 = 90, CCT9,
        FRC_TUNE = 92,
        //93
        NVM = 94,
        PERFORMANCE_COUNTER = 95,
        //96
        ECCSB_ERR = 97,
        DMA0 = 98, DMA1, DMA2, DMA3 = 101,
        END = 255
    };

    void disable_all( void ){       __builtin_disable_interrupts(); }
    void enable_all( void ){        __builtin_enable_interrupts(); }

    void proxtimer( uint8_t pri )
    {
        Reg::clr( BASE_ADDR, 7<<8 );
        Reg::set( BASE_ADDR, (pri&7)<<8 );
    }
    void eint4_rising( void ){      Reg::set( BASE_ADDR, INT4EP ); }
    void eint4_falling( void ){     Reg::clr( BASE_ADDR, INT4EP ); }
    void eint3_rising( void ){      Reg::set( BASE_ADDR, INT3EP ); }
    void eint3_falling( void ){     Reg::clr( BASE_ADDR, INT3EP ); }
    void eint2_rising( void ){      Reg::set( BASE_ADDR, INT2EP ); }
    void eint2_falling( void ){     Reg::clr( BASE_ADDR, INT2EP ); }
    void eint1_rising( void ){      Reg::set( BASE_ADDR, INT1EP ); }
    void eint1_falling( void ){     Reg::clr( BASE_ADDR, INT1EP ); }
    void eint0_rising( void ){      Reg::set( BASE_ADDR, INT0EP ); }
    void eint0_falling( void ){     Reg::clr( BASE_ADDR, INT0EP ); }

    //note- the following offsets calculated in bytes as the register
    //numbers are enums (not uint32_t*)- so need to calculate bytes
    //the Reg::set/clr will reinterpret the enums to volatile uint32_t*

    //vector 40 example
    //reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1
    //bit offset = 1<<(40%32) = 1<<8
    //bit offset 8 in IFS1
    void flagclear( Irq::IRQ_VN irqvn )
    {
        Reg::clr( Irq::BASE_ADDR_IFS + ((irqvn/32)*16), 1<<(irqvn%32) );
    }

    void enable( Irq::IRQ_VN irqvn )
    {
        flagclear( irqvn );
        Reg::set( Irq::BASE_ADDR_IEC + ((irqvn/32)*16), 1<<(irqvn%32) );
    }

    void disable( Irq::IRQ_VN irqvn )
    {
        Reg::clr( Irq::BASE_ADDR_IEC + ((irqvn/32)*16), 1<<(irqvn%32) );
    }

    //vector 17 example
    //priority_shift = 8*(17%4) =  8*1= 8
    //reg = 0xBF80F140 + (17/4)*16 = 0xBF80F180 = IPC4
    //priority bits at bit offset 8 in IPC4

    //init specific irq
    void init( Irq::IRQ_VN irqvn, uint8_t p, uint8_t s, bool en )
    {
        uint32_t priority_shift = 8*(irqvn%4);
        p &= 7; s &= 3; p <<= 2; p |= s;
        Reg::clr( Irq::BASE_ADDR_IPC + ((irqvn/4)*16), (31<<priority_shift));
        Reg::set( Irq::BASE_ADDR_IPC + ((irqvn/4)*16),
                (p<<priority_shift));
        if( en ) enable( irqvn );
    }

    //to create a list (array) of irq's to init/enable
    typedef struct {
        Irq::IRQ_VN irqvn;
        uint8_t p;
        uint8_t s;
        bool en;
    } irq_list_t;

    //init list (array) of irq's
    void init( irq_list_t* arr )
    {
        for( ; arr->irqvn != END; arr++ ){
            init( arr->irqvn, arr->p, arr->s, arr->en );
        }
    }

};

#endif /* _IRQ_H */