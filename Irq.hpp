#pragma once

//Interrupt

#include <cstdint>
#include "Reg.hpp"

struct Irq : private Reg {

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

            //irq vector numbers
            enum
IRQ_VN      : uint8_t {
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

            static auto
flag_clr    (IRQ_VN) -> void;

            static auto
flag        (IRQ_VN) -> bool;

            static auto
on          (IRQ_VN, bool) -> void;

            static auto
init        (IRQ_VN, uint8_t, uint8_t, bool) -> void;

            //to create a list (array) of irq's to init/enable
            using
            irq_list_t = struct {
            IRQ_VN irqvn;   //vector number
            uint8_t p;      //priority
            uint8_t s;      //sub-priority
            bool en;        //enable
            };

            static auto
init        (irq_list_t*, uint8_t) -> void;

            static auto
shadow_set  (uint8_t) -> void;

            using
isrfunc_t   = void(*)(void);

            static auto
isr_func    (IRQ_VN, isrfunc_t) -> void;

            static auto
isr         () -> void;

    private:

            static isrfunc_t m_isrfuncs[DMA3+1];

};



