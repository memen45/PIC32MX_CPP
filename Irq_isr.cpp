#include "Irq.hpp"

/*
 * TODO: Make function declarations weak, so override is 
 * possible from anywhere - these definitions are just 
 * here to make life easier 
 */

////////////////////////////////////////////////////////////////////////////////
// ISR MACRO
// usage- ISR_DEFAULT(ADC)
#define ISR_DEFAULT(nam) extern "C" __attribute((vector((int)Irq::m_lookup_vn[Irq::nam]),interrupt))  \
    void nam##_ISR() {                                                      \
        Irq::isr(Irq::m_lookup_vn[Irq::nam]);                               \
    }                                                                       \

ISR_DEFAULT(TIMER_2)
ISR_DEFAULT(TIMER_3)
ISR_DEFAULT(TIMER_4)
ISR_DEFAULT(TIMER_5) 
        
ISR_DEFAULT(SPI1_ERR) /* ISR_DEFAULT(SPI1_RX) and ISR_DEFAULT(SPI1_TX) have same vector number */ 
        
ISR_DEFAULT(I2C1_BUS) /* ISR_DEFAULT(I2C1_SLAVE) and ISR_DEFAULT(I2C1_MASTER) have same vector number */
        
ISR_DEFAULT(UART1_ERR) /* ISR_DEFAULT(UART1_TX) and ISR_DEFAULT(UART1_RX) have same vector number */ 
ISR_DEFAULT(UART2_ERR) /* ISR_DEFAULT(UART2_TX) and ISR_DEFAULT(UART2_RX) have same vector number */ 
ISR_DEFAULT(UART3_ERR) /* ISR_DEFAULT(UART3_TX) and ISR_DEFAULT(UART3_RX) have same vector number */ 
ISR_DEFAULT(UART4_ERR) /* ISR_DEFAULT(UART4_TX) and ISR_DEFAULT(UART4_RX) have same vector number */ 
ISR_DEFAULT(UART5_ERR) /* ISR_DEFAULT(UART5_TX) and ISR_DEFAULT(UART5_RX) have same vector number */ 
ISR_DEFAULT(UART6_ERR) /* ISR_DEFAULT(UART6_TX) and ISR_DEFAULT(UART6_RX) have same vector number */ 

ISR_DEFAULT(USB)
