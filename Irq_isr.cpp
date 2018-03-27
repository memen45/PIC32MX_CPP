#include "Irq_isr.hpp"
#include "Irq.hpp"

/*
 * TODO: Make function declarations weak, so override is 
 * possible from anywhere - these definitions are just 
 * here to make life easier 
 */

////////////////////////////////////////////////////////////////////////////////
// ISR MACRO
// usage- ISR_DEFAULT(ADC)
#define ISR_DEFAULT(nam) extern "C" __attribute((vector((int)(Irq::m_lookup_vn[Irq::nam])),interrupt))  \
    void nam##_ISR() {                                                      \
        if (isr_callback[Irq::m_lookup_vn[Irq::nam]])                       \
        isr_callback[Irq::m_lookup_vn[Irq::nam]]();                         \
    }                                                                       \

ISR_DEFAULT(TIMER_2)
ISR_DEFAULT(TIMER_3)
ISR_DEFAULT(TIMER_4)
ISR_DEFAULT(TIMER_5) 
ISR_DEFAULT(UART1_TX) /* ISR_DEFAULT(UART1_ERROR) and ISR_DEFAULT(UART1_RX) have same vector number */ 
ISR_DEFAULT(UART2_TX) /* ISR_DEFAULT(UART2_ERROR) and ISR_DEFAULT(UART2_RX) have same vector number */ 
ISR_DEFAULT(UART3_TX) /* ISR_DEFAULT(UART3_ERROR) and ISR_DEFAULT(UART3_RX) have same vector number */ 
ISR_DEFAULT(UART4_TX) /* ISR_DEFAULT(UART4_ERROR) and ISR_DEFAULT(UART4_RX) have same vector number */ 
ISR_DEFAULT(UART5_TX) /* ISR_DEFAULT(UART5_ERROR) and ISR_DEFAULT(UART5_RX) have same vector number */ 
ISR_DEFAULT(UART6_TX) /* ISR_DEFAULT(UART6_ERROR) and ISR_DEFAULT(UART6_RX) have same vector number */ 
