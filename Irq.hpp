#ifndef _IRQ_H
#define _IRQ_H

#include <cstdint>
#include"Reg.hpp"

//Irq applies to all Irqn
namespace Irq {
    //proximity timer control
    enum IRQ_TPC : uint8_t {
        TPC_0 = 0, TPC_1, TPC_2, TPC_3, TPC_4, TPC_5, TPC_6, TPC_7
    };
    //priority,subpriority
    enum IRQ_PRI : uint8_t {
        PRI0 = 0, PRI1 = 1<<2, PRI2 = 2<<2, PRI3 = 3<<2, PRI4 = 4<<2,
        PRI5 = 5<<2, PRI6 = 6<<2, PRI7 = 7<<2
    };
    enum IRQ_SUB : uint8_t {
        SUB0 = 0, SUB1, SUB2, SUB3
    };
    //other
    enum {
        BASE_ADDR = 0xBF80F000UL,
        INT0EP = 1<<0, INT1EP = 1<<1, INT2EP = 1<<2,
        INT3EP = 1<<3, INT4EP = 1<<4
    };
    //irq vector numbers
    enum IRQVN : uint8_t {
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
        DMA0 = 98, DMA1, DMA2, DMA3 = 101
    };

    void disable_all( void ){   __builtin_disable_interrupts(); }
    void enable_all( void ){    __builtin_enable_interrupts(); }

    void proxtimer( IRQ_TPC );
    void eint4_rising( void );
    void eint4_falling( void );
    void eint3_rising( void );
    void eint3_falling( void );
    void eint2_rising( void );
    void eint2_falling( void );
    void eint1_rising( void );
    void eint1_falling( void );
    void eint0_rising( void );
    void eint0_falling( void );
};

//specific to each irq
class Irqn {

    public:
        Irqn( Irq::IRQVN irqvn, Irq::IRQ_PRI pri, Irq::IRQ_SUB sub ) :
            m_ifs( (volatile uint32_t*)Irq::BASE_ADDR + 16 + ((irqvn/32)*4) ),
            m_iec( (volatile uint32_t*)Irq::BASE_ADDR + 48 + ((irqvn/32)*4) ),
            m_ipc( (volatile uint32_t*)Irq::BASE_ADDR + 80 + ((irqvn/4)*4) ),
            m_irq_bit( 1 << (irqvn%32) ),
            m_priority_shift( 8*(irqvn%4) )
        {
            priority( pri, sub );
        }

    public:
        void enable( void ){        flagclear(); Reg::set(m_iec,m_irq_bit); }
        void disable( void ){       Reg::clr(m_iec,m_irq_bit); }
        void flagclear( void ){     Reg::clr(m_ifs,m_irq_bit); }
        void priority( Irq::IRQ_PRI p, Irq::IRQ_SUB s = Irq::IRQ_SUB::SUB0 ){
            Reg::clr(m_ipc,(31<<m_priority_shift));
            Reg::set(m_ipc,((p|s)<<m_priority_shift));
        }

    private:
        volatile uint32_t* m_ifs;
        volatile uint32_t* m_iec;
        volatile uint32_t* m_ipc;
        const uint32_t m_irq_bit;
        const uint8_t m_priority_shift;
};

#endif /* _IRQ_H */