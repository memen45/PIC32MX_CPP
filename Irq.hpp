#pragma once

//Interrupt

#include <cstdint>
#include <functional>

struct Irq {
    
            static auto
global      (bool) -> void;

            static auto
global      () -> bool;

            static auto
proxtimer   (uint8_t, uint32_t = 0) -> void;

            enum
EINTXPOL    : bool { FALLING, RISING };

            static auto
eint4_pol   (EINTXPOL) -> void;

            static auto
eint3_pol   (EINTXPOL) -> void;

            static auto
eint2_pol   (EINTXPOL) -> void;

            static auto
eint1_pol   (EINTXPOL) -> void;

            static auto
eint0_pol   (EINTXPOL) -> void;

            //irq numbers
            enum
IRQ_NR      : uint8_t {
            CORE_TIMER = 0, CORE_SOFTWARE_0, CORE_SOFTWARE_1,

            EXTERNAL_0 = 3, TIMER_1 = 4, 	INPUT_CAPTURE_1 = 5, 	OUTPUT_COMPARE_1 = 6,
            EXTERNAL_1, 	TIMER_2, 		INPUT_CAPTURE_2, 		OUTPUT_COMPARE_2,
            EXTERNAL_2, 	TIMER_3, 		INPUT_CAPTURE_3, 		OUTPUT_COMPARE_3,
            EXTERNAL_3, 	TIMER_4, 		INPUT_CAPTURE_4, 		OUTPUT_COMPARE_4,
            EXTERNAL_4, 	TIMER_5, 		INPUT_CAPTURE_5, 		OUTPUT_COMPARE_5,

            SPI1_ERR = 23, SPI1_RX = 24, SPI1_TX = 25,

            UART1_ERR = 26, SPI3_ERR = 26, I2C3_BUS = 26,
            UART1_RX = 27, SPI3_RX = 27, I2C3_SLAVE = 27,
            UART1_TX = 28, SPI3_TX = 28, I2C3_MASTER = 28,

            I2C1_BUS = 29, I2C1_SLAVE, I2C1_MASTER,
            CHANGE_NOTICE = 32,
            ADC = 33,
            PMP = 34,
            CMP_1 = 35, CMP_2,

            UART3_ERR = 37, SPI2_ERR = 37, I2C4_BUS = 37,
            UART3_RX = 38, SPI2_RX = 38, I2C4_SLAVE = 38,
            UART3_TX = 39, SPI2_TX = 39, I2C4_MASTER = 39,

            UART2_ERR = 40, SPI4_ERR = 40, I2C5_BUS = 40,
            UART2_RX = 41, SPI4_RX = 41, I2C5_SLAVE = 41,
            UART2_TX = 42, SPI4_TX = 42, I2C5_MASTER = 42,

            I2C2_BUS = 43, I2C2_SLAVE, I2C2_MASTER,

            FSCM = 46,
            RTCC = 47,
            DMA0 = 48, DMA1, DMA2, DMA3, DMA4, DMA5, DMA6, DMA7,
            FCE = 56,
            USB = 57,
            CAN1 = 58, CAN2,
            ETHERNET = 60,

            INPUT_CAPTURE_1_ERROR = 61, INPUT_CAPTURE_2_ERROR, INPUT_CAPTURE_3_ERROR, INPUT_CAPTURE_4_ERROR, INPUT_CAPTURE_5_ERROR,
            PMP_ERROR = 66, 

            UART4_ERR = 67, UART4_RX, UART4_TX,
            UART6_ERR = 70, UART6_RX, UART6_TX,	
            UART5_ERR = 73, UART5_RX, UART5_TX,
            };
    
            static constexpr uint8_t 
m_lookup_vn[] = {
            /*CORE_TIMER = */ 0, /* CORE_SOFTWARE_0 = */ 1, /* CORE_SOFTWARE_1 = */ 2,

            /* EXTERNAL_0 = */ 3, /* TIMER_1 = */ 4, 	/* INPUT_CAPTURE_1 =  */5, /* OUTPUT_COMPARE_1 = */ 6,
            /* EXTERNAL_1 = */ 7, 	/* TIMER_2 = */ 8, 		/* INPUT_CAPTURE_2 = */ 9, 		/* OUTPUT_COMPARE_2 = */ 10,
            /* EXTERNAL_2 = */ 11, 	/* TIMER_3 = */ 12, 		/* INPUT_CAPTURE_3 = */ 13, 		/* OUTPUT_COMPARE_3 = */ 14,
            /* EXTERNAL_3 = */ 15, 	/* TIMER_4 = */ 16, 		/* INPUT_CAPTURE_4 = */ 17, 		/* OUTPUT_COMPARE_4 = */ 18,
            /* EXTERNAL_4 = */ 19, 	/* TIMER_5 = */ 20, 		/* INPUT_CAPTURE_5 = */ 21, 		/* OUTPUT_COMPARE_5 = */ 22,

            /* SPI1_ERR = */ 23, /* SPI1_RX = */ 23, /* SPI1_TX = */ 23,

            /* UART1_ERR = */ 24, /* SPI3_ERR = 24, I2C3_BUS =  24, */
            /* UART1_RX = */ 24, /* SPI3_RX = 24, I2C3_SLAVE = 24, */
            /* UART1_TX = */ 24, /* SPI3_TX = 24, I2C3_MASTER = 24, */

            /* I2C1_BUS = */ 25, /* I2C1_SLAVE = */ 25, /* I2C1_MASTER = */ 25,
            /* CHANGE_NOTICE = */ 26,
            /* ADC = */ 27,
            /* PMP = */ 28,
            /* CMP_1 = */ 29, /* CMP_2 = */ 30,

            /* UART3_ERR = */ 31, /* SPI2_ERR = 31, I2C4_BUS = 31, */
            /* UART3_RX = */ 31, /* SPI2_RX = 31, I2C4_SLAVE = 31, */
            /* UART3_TX = */ 31, /* SPI2_TX = 31, I2C4_MASTER = 31, */

            /* UART2_ERR = */ 32, /* SPI4_ERR = 32, I2C5_BUS = 32, */
            /* UART2_RX = */ 32, /* SPI4_RX = 32, I2C5_SLAVE = 32, */
            /* UART2_TX = */ 32, /* SPI4_TX = 32, I2C5_MASTER = 32, */

            /* I2C2_BUS = */ 33, /* I2C2_SLAVE = */ 33, /* I2C2_MASTER = */ 33,

            /* FSCM = */ 34,
            /* RTCC = */ 35,
            /* DMA0 = */ 36, /* DMA1 = */ 37, /* DMA2 = */ 38, /* DMA3 = */ 39, /* DMA4 = */ 40, /* DMA5 = */ 41, /* DMA6 = */ 42, /* DMA7 = */ 43,
            /* FCE = */ 44,
            /* USB = */ 45,
            /* CAN1 = */ 46, /* CAN2 = */ 47,
            /* ETHERNET = */ 48,

            /* INPUT_CAPTURE_1_ERROR = */ 5, /* INPUT_CAPTURE_2_ERROR = */ 9, /* INPUT_CAPTURE_3_ERROR = */ 13, /* INPUT_CAPTURE_4_ERROR = */ 17, /* INPUT_CAPTURE_5_ERROR = */ 21,
            /* PMP_ERROR = */ 28, 

            /* UART4_ERR = */ 49, /* UART4_RX = */ 49, /* UART4_TX = */ 49,
            /* UART6_ERR = */ 50, /* UART6_RX = */ 50, /* UART6_TX = */ 50,	
            /* UART5_ERR = */ 51, /* UART5_RX = */ 51, /* UART5_TX = */ 51,
            };

            static auto
flag_clr    (IRQ_NR) -> void;

            static auto
flag        (IRQ_NR) -> bool;

            static auto
on          (IRQ_NR, bool) -> void;

            static auto
on          (IRQ_NR) -> bool;

            static auto
init        (IRQ_NR, uint8_t, uint8_t, bool) -> void;

            //to create a list (array) of irq's to init/enable
            using
            irq_list_t = struct {
            IRQ_NR irqvn;   //vector number
            uint8_t p;      //priority
            uint8_t s;      //sub-priority
            bool en;        //enable
            };

            static auto
init        (irq_list_t*, uint8_t) -> void;
            
            static auto
isr_fun     (IRQ_NR, std::function<void()>) -> void;
            
            enum 
MVEC_MODE   : bool { 
            SVEC = false, MVEC 
            };
            
            static auto
enable_mvec (MVEC_MODE) -> void;

};

////////////////////////////////////////////////////////////////////////////////
// ISR MACRO
// usage- ISR(ADC){ /* isr code here */ }
#define ISR(nam) extern "C" __attribute((vector((int)Irq::nam),interrupt))\
    void nam##_ISR()

////////////////////////////////////////////////////////////////////////////////
// ISR MACRO - auto clear irq flag
// usage- ISRautoflag(ADC){ /* isr code here */ }}
// NOTE- need }} to close isr
// NOTE flag is cleared at start of isr, do not use if flag needs clearing
// after a register is read
#define ISRautoflag(nam) extern "C" \
    __attribute((vector((int)Irq::nam),interrupt)) \
    void nam##_ISR(){ Irq::flag_clr(Irq::nam); \

