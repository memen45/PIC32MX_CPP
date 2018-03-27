#include "Irq_isr.hpp"
#include "Irq.hpp"

/*
 * TODO: Make function declarations weak, so override is 
 * possible from anywhere - these definitions are just 
 * here to make life easier 
 * TODO: Make function declaration even more generic, 
 * making it even easier to add vectors to this list of 
 * declarations
 */

////////////////////////////////////////////////////////////////////////////////
// ISR MACRO
// usage- ISR(ADC)
#define ISR_DEFAULT(nam) extern "C" __attribute((vector((int)Irq::nam),interrupt))  \
    void nam##_ISR() {                                                      \
        if (isr_callback[Irq::m_lookup_vn[Irq::nam]])                       \
        isr_callback[Irq::m_lookup_vn[Irq::nam]]();                         \
    }                                                                       \

ISR(TIMER_2) {
    if (isr_callback[Irq::m_lookup_vn[Irq::TIMER_2]])
        isr_callback[Irq::m_lookup_vn[Irq::TIMER_2]]();
}

ISR_DEFAULT(TIMER_3)

ISR(TIMER_4) {
    if (isr_callback[Irq::m_lookup_vn[Irq::TIMER_4]])
        isr_callback[Irq::m_lookup_vn[Irq::TIMER_4]]();
}
ISR(TIMER_5) {
    if (isr_callback[Irq::m_lookup_vn[Irq::TIMER_5]])
        isr_callback[Irq::m_lookup_vn[Irq::TIMER_5]]();
}
