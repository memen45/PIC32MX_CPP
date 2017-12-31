#pragma once

/*=============================================================================
 Interrupt functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"


struct Irq {

    //irq vector numbers
    enum IRQ_VN : uint8_t {
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
        DMA0 = 98, DMA1, DMA2, DMA3 = 101,
        END = 255
    };

    //to create a list (array) of irq's to init/enable
    typedef struct {
        IRQ_VN irqvn;   //vector number
        uint8_t p;      //priority
        uint8_t s;      //sub-priority
        bool en;        //enable
    } irq_list_t;


    //public functions
    static void     disable_all     ();
    static void     enable_all      ();
    static bool     all_ison        ();
    static void     proxtimer       (uint8_t);
    static void     eint4_rising    (bool);
    static void     eint3_rising    (bool);
    static void     eint2_rising    (bool);
    static void     eint1_rising    (bool);
    static void     eint0_rising    (bool);
    static void     flag_clr        (IRQ_VN);
    static bool     flag            (IRQ_VN);
    static void     on              (IRQ_VN, bool);
    static void     init            (IRQ_VN, uint8_t, uint8_t, bool);
    static void     init            (irq_list_t*);
    static void     shadow_set      (uint8_t, bool);


    private:

    static Reg r;

    enum {
        INTCON = 0xBF80F000,
        PRISS = 0xBF80F010,
        INTSTAT = 0xBF80F020,
        IFS_BASE = 0xBF80F040,
        IEC_BASE = 0xBF80F0C0,
        IPC_BASE = 0xBF80F140,
        INT0EP = 1<<0, INT1EP = 1<<1, INT2EP = 1<<2,
        INT3EP = 1<<3, INT4EP = 1<<4,
        TPCMASK = 7, TPCSHIFT = 8,
    };
};

/*=============================================================================
 all functions inline
=============================================================================*/

void Irq::disable_all(){ __builtin_disable_interrupts(); }
void Irq::enable_all(){ __builtin_enable_interrupts(); }
bool Irq::all_ison(){ return (__builtin_mfc0(12,0) & 1); }
void Irq::proxtimer(uint8_t pri){
    r.clr(INTCON, TPCMASK<<TPCSHIFT);
    r.set(INTCON, (pri&TPCMASK)<<TPCSHIFT);
}
void Irq::eint4_rising(bool tf){ r.set(INTCON, INT4EP, tf); }
void Irq::eint3_rising(bool tf){ r.set(INTCON, INT3EP, tf); }
void Irq::eint2_rising(bool tf){ r.set(INTCON, INT2EP, tf); }
void Irq::eint1_rising(bool tf){ r.set(INTCON, INT1EP, tf); }
void Irq::eint0_rising(bool tf){ r.set(INTCON, INT0EP, tf); }

//note- the following offsets calculated in bytes as the register
//numbers are enums (not uint32_t*)- so need to calculate bytes
//the Reg::set/clr will reinterpret the enums to volatile uint32_t*
//
//vector 40 example
//reg = 0xBF80F040 + (40/32)*16 = 0xBF80F050 = IFS1
//bit offset = 1<<(40%32) = 1<<8
//bit offset 8 in IFS1
void Irq::flag_clr(IRQ_VN irqvn){
    r.clr(IFS_BASE + ((irqvn/32)*16), 1<<(irqvn%32));
}
bool Irq::flag(IRQ_VN irqvn){
    return r.is_set(IFS_BASE + ((irqvn/32)*16), 1<<(irqvn%32));
}
void Irq::on(IRQ_VN irqvn, bool tf){
    r.set(IEC_BASE + ((irqvn/32)*16), 1<<(irqvn%32), tf);
}
//vector 17 example
//priority_shift = 8*(17%4) =  8*1= 8
//reg = 0xBF80F140 + (17/4)*16 = 0xBF80F180 = IPC4
//priority bits at bit offset 8 in IPC4
//
//init specific irq
//pri is masked to 0-7, sub is masked to 0-3
//pri of 0 disables irq
void Irq::init(IRQ_VN irqvn, uint8_t pri, uint8_t sub, bool en){
    uint32_t priority_shift = 8*(irqvn%4);
    pri &= 7; sub &= 3; pri <<= 2; pri |= sub;
    r.clr(IPC_BASE + ((irqvn/4)*16), (31<<priority_shift));
    r.set(IPC_BASE + ((irqvn/4)*16), (pri<<priority_shift));
    flag_clr(irqvn);
    on(irqvn, en);
}
//init list (array) of irq's
void Irq::init(irq_list_t* arr){
    for(; arr->irqvn != END; arr++){
        init(arr->irqvn, arr->p, arr->s, arr->en);
    }
}
//priority shadow set, 0 or 1
//pri = 1-7, tf =0,1 (0=normal, 1=shadow)
//pri is masked to 0-7
//pri val of 0 will set/clr SS0- no harm as not using
void Irq::shadow_set(uint8_t pri, bool tf){
    pri &= 7; pri <<= 2; //0*4=0 1*4=4, 7*4=28
    r.set(PRISS, 1<<pri, tf);
}


////////////////////////////////////////////////////////////////////////////////
// ISR MACRO
////////////////////////////////////////////////////////////////////////////////
//usage- ISR(ADC){ /* isr code here */ }
#define ISR(nam) extern "C" __attribute((vector((int)Irq::nam),interrupt))\
    void nam##_ISR()
