#pragma once

//Interrupt

#include <cstdint>
#include <functional>

struct Irq {

    static void     disable_all     ();
    static void     enable_all      ();
    enum MVEC_MODE : bool { SVEC = false, MVEC };
    static void     enable_mvec     (MVEC_MODE);
    static bool     all_ison        ();
    static void     proxtimer       (uint8_t);

    enum EINTXPOL : bool { FALLING = 0, RISING };
    static void     eint4_pol       (EINTXPOL);
    static void     eint3_pol       (EINTXPOL);
    static void     eint2_pol       (EINTXPOL);
    static void     eint1_pol       (EINTXPOL);
    static void     eint0_pol       (EINTXPOL);

	//irq numbers
    enum IRQ_NR : uint8_t {
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
		END = 255
    };

    static void     flag_clr        (IRQ_NR);
    static bool     flag            (IRQ_NR);
    static void     on              (IRQ_NR, bool);
    static void     init            (IRQ_NR, uint8_t, uint8_t, bool);
    static void     init            (IRQ_NR, uint8_t, uint8_t, bool, std::function<void()>);
    

    //to create a list (array) of irq's to init/enable
    using irq_list_t = struct {
        IRQ_NR irqvn;   //vector number
        uint8_t p;      //priority
        uint8_t s;      //sub-priority
        bool en;        //enable
    };
    static void     init            (irq_list_t*);
	
	static const uint8_t m_lookup_vn[95];

};

////////////////////////////////////////////////////////////////////////////////
// ISR MACRO - auto clear irq flag - NOTE- need }} to close isr
// usage- ISR(ADC){ /* isr code here */ }}
#define ISRautoflag(nam) extern "C" \
    __attribute((vector((int)Irq::nam),interrupt)) \
    void nam##_ISR(){ Irq::flag_clr(Irq::nam); \


