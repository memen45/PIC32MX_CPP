/* 
 * File:   Irq_isr.h
 * Author: beheerder
 *
 * Created on 10 maart 2018, 20:35
 */

#ifndef IRQ_ISR_HPP
#define	IRQ_ISR_HPP

#include <functional>

std::function<void()> isr_callback[52];

#endif	/* IRQ_ISR_HPP */

