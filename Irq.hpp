#ifndef _IRQ_H
#define _IRQ_H

#include <cstdint>

//Irq applies to all Irqn
namespace Irq {
    //proximity timer control
    enum IRQ_TPC : uint8_t {
        TPC_0 = 0, TPC_1 = 1, TPC_2 = 2, TPC_3 = 3,
        TPC_4 = 4, TPC_5 = 5, TPC_6 = 6, TPC_7 = 7,
    };
    //priority,subpriority
    enum IRQ_PRI : uint8_t {
        PRI0 = 0, PPR1 = 1<<2, PRI2 = 2<<2, PRI3 = 3<<2, PRI4 = 4<<2,
        PRI5 = 5<<2, PRI6 = 6<<2, PRI7 = 7<<2
    };
    enum IRQ_SUB : uint8_t {
        SUB0 = 0, SUB1 = 1, SUB2 = 2, SUB3 = 3
    };
    //other
    enum {
        BASE_ADDR = 0xBF80F000UL,
        INT0EP = 0, INT1EP = 1, INT2EP = 2, INT3EP = 3, INT4EP = 4
    };
    //irq vector numbers
    enum IRQVN : uint8_t {
        CORE_TIMER = 0,
        CORE_SOFTWARE_0 = 1,
        CORE_SOFTWARE_1 = 2,
        EXTERNAL_0 = 3,
        EXTERNAL_1 = 4,
        EXTERNAL_2 = 5,
        EXTERNAL_3 = 6,
        EXTERNAL_4 = 7,
        CHANGE_NOTICE_A = 8,
        CHANGE_NOTICE_B = 9,
        CHANGE_NOTICE_C = 10,
        CHANGE_NOTICE_D = 11,
        TIMER_1 = 17,
        TIMER_2 = 18,
        TIMER_3 = 19,
        COMPARATOR_1 = 23,
        COMPARATOR_2 = 24,
        COMPARATOR_3 = 25,
        USB = 29,
        RTCC = 32,
        ADC = 33,
        HLVD = 36,
        CLC1 = 37,
        CLC2 = 38,
        CLC3 = 39,
        CLC4 = 40,
        SPI1_ERR = 41,
        SPI1_TX = 42,
        SPI1_RX = 43,
        SPI2_ERR = 44,
        SPI2_TX = 45,
        SPI2_RX = 46,
        SPI3_ERR = 47,
        SPI3_TX = 48,
        SPI3_RX = 49,
        UART1_RX = 53,
        UART1_TX = 54,
        UART1_ERR = 55,
        UART2_RX = 56,
        UART2_TX = 57,
        UART2_ERR = 58,
        UART3_RX = 59,
        UART3_TX = 60,
        UART3_ERR = 61,
        I2C1_SLAVE = 65,
        I2C1_MASTER = 66,
        I2C1_BUS = 67,
        I2C2_SLAVE = 68,
        I2C2_MASTER = 69,
        I2C2_BUS = 70,
        I2C3_SLAVE = 71,
        I2C3_MASTER = 72,
        I2C3_BUS = 73,
        CCP1 = 74,
        CCT1 = 75,
        CCP2 = 76,
        CCT2 = 77,
        CCP3 = 78,
        CCT3 = 79,
        CCP4 = 80,
        CCT4 = 81,
        CCP5 = 82,
        CCT5 = 83,
        CCP6 = 84,
        CCT6 = 85,
        CCP7 = 86,
        CCT7 = 87,
        CCP8 = 88,
        CCT8 = 89,
        CCP9 = 90,
        CCT9 = 91,
        FRC_TUNE = 92,
        NVM = 94,
        PERFORMANCE_COUNTER = 95,
        ECCSB_ERR = 97,
        DMA0 = 98,
        DMA1 = 99,
        DMA2 = 100,
        DMA3 = 101
    };

    void proxtimer( IRQ_TPC );
    void eint4_rising();
    void eint4_falling();
    void eint3_rising();
    void eint3_falling();
    void eint2_rising();
    void eint2_falling();
    void eint1_rising();
    void eint1_falling();
    void eint0_rising();
    void eint0_falling();
};

//specific to each irq
class Irqn {

    public:
        Irqn( Irq::IRQVN irqvn, Irq::IRQ_PRI pri, Irq::IRQ_SUB sub ) :
            m_ifs( (uint32_t*)Irq::BASE_ADDR + 16 + ((irqvn/32)*4) ),
            m_iec( (uint32_t*)Irq::BASE_ADDR + 32 + ((irqvn/32)*4) ),
            m_ipc( (uint32_t*)Irq::BASE_ADDR + 48 + ((irqvn/4)*4) ),
            m_irq_bit( 1 << (irqvn%32) ),
            m_priority_shift( 1<< (irqvn%4) )
        {
            priority( pri, sub );
        }

    public:
        void enable( void ){        flagclear(); *(m_iec+1) = m_irq_bit; }
        void disable( void ){       *(m_iec+1) = m_irq_bit; }
        void flagclear( void ){     *(m_ifs+1) = m_irq_bit; }
        void priority( Irq::IRQ_PRI p, Irq::IRQ_SUB s = Irq::IRQ_SUB::SUB0 ){
            *(m_ipc+1) = (31<<m_priority_shift); //CLR
            *(m_ipc+2) = ((p|s)<<m_priority_shift); //SET
        }

    private:
        volatile uint32_t* m_ifs;
        uint32_t* m_iec;
        uint32_t* m_ipc;
        uint8_t m_irq_bit;
        uint8_t m_priority_shift;
};

#endif /* _IRQ_H */