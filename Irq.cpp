#include "Irq.hpp"
#include "Reg.hpp"

enum {
    INTCON = 0xBF881000,
        TPC_SHIFT = 8, TPC_CLR = 7,
        INT4EP = 4,
        INT3EP = 3,
        INT2EP = 2,
        INT1EP = 1,
        INT0EP = 0,
    INTSTAT = 0xBF881010,
    IFS_BASE = 0xBF881030,
    IEC_BASE = 0xBF881060,
    IPC_BASE = 0xBF881090
};

const uint8_t Irq::m_lookup_vn[] = {
        /*CORE_TIMER = */ 0, /* CORE_SOFTWARE_0 = */ 1, /* CORE_SOFTWARE_1 = */ 2,
		
		/* EXTERNAL_0 = */ 3, /* TIMER_1 = */ 4, 	/* INPUT_CAPTURE_1 =  */5, /* OUTPUT_COMPARE_1 = */ 6,
		/* EXTERNAL_1 = */ 7, 	/* TIMER_2 = */ 8, 		/* INPUT_CAPTURE_2 = */ 9, 		/* OUTPUT_COMPARE_2 = */ 10,
		/* EXTERNAL_2 = */ 11, 	/* TIMER_3 = */ 12, 		/* INPUT_CAPTURE_3 = */ 13, 		/* OUTPUT_COMPARE_3 = */ 14,
		/* EXTERNAL_3 = */ 15, 	/* TIMER_4 = */ 16, 		/* INPUT_CAPTURE_4 = */ 17, 		/* OUTPUT_COMPARE_4 = */ 18,
		/* EXTERNAL_4 = */ 19, 	/* TIMER_5 = */ 20, 		/* INPUT_CAPTURE_5 = */ 21, 		/* OUTPUT_COMPARE_5 = */ 22,
		
		/* SPI1_ERR = */ 23, /* SPI1_RX = */ 23, /* SPI1_TX = */ 23,
		
		/* UART1_ERR = */ 24, /* SPI3_ERR = */ 24, /* I2C3_BUS = */ 24,
		/* UART1_RX = */ 24, /* SPI3_RX = */ 24, /* I2C3_SLAVE = */ 24,
		/* UART1_TX = */ 24, /* SPI3_TX = */ 24, /* I2C3_MASTER = */ 24,
		
		/* I2C1_BUS = */ 25, /* I2C1_SLAVE = */ 25, /* I2C1_MASTER = */ 25,
		/* CHANGE_NOTICE = */ 26,
		/* ADC = */ 27,
		/* PMP = */ 28,
		/* CMP_1 = */ 29, /* CMP_2 = */ 30,
		
		/* UART3_ERR = */ 31, /* SPI2_ERR = */ 31, /* I2C4_BUS = */ 31,
		/* UART3_RX = */ 31, /* SPI2_RX = */ 31, /* I2C4_SLAVE = */ 31,
		/* UART3_TX = */ 31, /* SPI2_TX = */ 31, /* I2C4_MASTER = */ 31,
		
		/* UART2_ERR = */ 32, /* SPI4_ERR = */ 32, /* I2C5_BUS = */ 32,
		/* UART2_RX = */ 32,/*  SPI4_RX = */ 32, /* I2C5_SLAVE = */ 32,
		/* UART2_TX = */ 32, /* SPI4_TX = */ 32, /* I2C5_MASTER = */ 32,
		
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
		/* UART5_ERR = */ 51, /* UART5_RX = */ 51, /* UART5_TX = */ 51
    };

//=============================================================================
    void        Irq::disable_all        ()
//=============================================================================
{
    __builtin_disable_interrupts();
}

//=============================================================================
    void        Irq::enable_all         ()
//=============================================================================
{
    __builtin_enable_interrupts();
}

//=============================================================================
    bool        Irq::all_ison           ()
//=============================================================================
{
    return (__builtin_mfc0(12, 0) bitand 1);
}

//=============================================================================
    void        Irq::proxtimer          (uint8_t n)
//=============================================================================
{ //n = priority 0-7
    Reg::clrbit(INTCON, TPC_CLR<<TPC_SHIFT);
    Reg::setbit(INTCON, (n bitand TPC_CLR)<<TPC_SHIFT);
}

//=============================================================================
    void        Irq::eint4_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT4EP, e);
}

//=============================================================================
    void        Irq::eint3_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT3EP, e);
}

//=============================================================================
    void        Irq::eint2_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT2EP, e);
}

//=============================================================================
    void        Irq::eint1_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT1EP, e);
}

//=============================================================================
    void        Irq::eint0_pol          (EINTXPOL e)
//=============================================================================
{
    Reg::setbit(INTCON, 1<<INT0EP, e);
}

//note- the following offsets calculated in bytes as the register
//numbers are enums (not uint32_t*)- so need to calculate bytes
//the Reg::set/clr will reinterpret the enums to volatile uint32_t*
//
//vector 40 example												IRQ number: 66
//reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1				+(66/32)*16 = +32
//bit offset = 1<<(40%32) = 1<<8								1<<(66%32) = 1<<2
//bit offset 8 in IFS1											offset 2 in IFS2
//=============================================================================
    void        Irq::flag_clr           (IRQ_NR e)
//=============================================================================
{
    Reg::clrbit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
}

//=============================================================================
    bool        Irq::flag               (IRQ_NR e)
//=============================================================================
{
    return Reg::anybit(IFS_BASE + ((e / 32) * 16), 1u<<(e % 32));
}

//=============================================================================
    void        Irq::on                 (IRQ_NR e, bool tf)
//=============================================================================
{
    Reg::setbit(IEC_BASE + ((e / 32) * 16), 1u<<(e % 32), tf);
}

//vector 17 example												Vector number: 28
//priority_shift = 8*(17%4) =  8*1= 8							8*(28%4) = 8*2 = 0
//reg = 0xBF80F140 + (17/4)*16 = 0xBF80F180 = IPC4				+ (28/4)*16 = +112
//priority bits at bit offset 8 in IPC4							offset 0 in IPC7
//
//init specific irq
//pri is masked to 0-7, sub is masked to 0-3
//pri of 0 disables irq
//=============================================================================
    void        Irq::init       (IRQ_NR e, uint8_t pri, uint8_t sub, bool tf)
//=============================================================================
{
	uint8_t vn = m_lookup_vn[e];
    uint32_t priority_shift = 8 * (vn % 4);
    pri and_eq 7; sub and_eq 3; pri <<= 2; pri or_eq sub;
    Reg::clrbit(IPC_BASE + ((vn / 4) * 16), (31<<priority_shift));
    Reg::setbit(IPC_BASE + ((vn / 4) * 16), (pri<<priority_shift));
    flag_clr(e);
    on(e, tf);
}

//init list (array) of irq's
//=============================================================================
    void        Irq::init               (irq_list_t* arr)
//=============================================================================
{
    for(; arr->irqvn not_eq END; arr++){
        init(arr->irqvn, arr->p, arr->s, arr->en);
    }
}


