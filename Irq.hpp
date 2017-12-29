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


//struct isr {
//    [[gnu::vector(5), gnu::keep, gnu::interrupt(IPL0AUTO) ]] void (*fp)();
//    uint8_t vn;
//    isr(uint8_t n, void(*f)()) :
//    vn(n),
//    fp(f)
//    {}
//};

//YUCK - DEFINES

////////////////////////////////////////////////////////////////////////////////
// ISR MACROS - the only (easy) way to 'automate' isr creation a little bit,
//              is to use defines/macros, as the __attribute__ options need
//              constants that cannot be computed except by simple substitution
//              by the preprocessor
////////////////////////////////////////////////////////////////////////////////
// these names for the isr are similar to IRQ_VN enum names, appending _VN
// BUT, you need to use the IRQ_VN name in the ISR macro, which will be
// converted to use these defines
////////////////////////////////////////////////////////////////////////////////
#define CORE_TIMER_VN 0
#define CORE_SOFTWARE_0_VN 1
#define CORE_SOFTWARE_1_VN 2
#define EXTERNAL_0_VN 3
#define EXTERNAL_1_VN 4
#define EXTERNAL_2_VN 5
#define EXTERNAL_3_VN 6
#define EXTERNAL_4_VN 7
#define CHANGE_NOTICE_A_VN 8
#define CHANGE_NOTICE_B_VN 9
#define CHANGE_NOTICE_C_VN 10
#define CHANGE_NOTICE_D_VN 11
#define TIMER_1_VN 17
#define TIMER_2_VN 18
#define TIMER_3_VN 19
#define COMPARATOR_1_VN 23
#define COMPARATOR_2_VN 24
#define COMPARATOR_3_VN 25
#define USB_VN 29
#define RTCC_VN 32
#define ADC_VN 33
#define HLVD_VN 36
#define CLC1_VN 37
#define CLC2_VN 38
#define CLC3_VN 39
#define CLC4_VN 40
#define SPI1_ERR_VN 41
#define SPI1_TX_VN 42
#define SPI1_RX_VN 43
#define SPI2_ERR_VN 44
#define SPI2_TX_VN 45
#define SPI2_RX_VN 46
#define SPI3_ERR_VN 47
#define SPI3_TX_VN 48
#define SPI3_RX_VN 49
#define UART1_RX_VN 53
#define UART1_TX_VN 54
#define UART1_ERR_VN 55
#define UART2_RX_VN 56
#define UART2_TX_VN 57
#define UART2_ERR_VN 58
#define UART3_RX_VN 59
#define UART3_TX_VN 60
#define UART3_ERR_VN 61
#define I2C1_SLAVE_VN 65
#define I2C1_MASTER_VN 66
#define I2C1_BUS_VN 67
#define I2C2_SLAVE_VN 68
#define I2C2_MASTER_VN 69
#define I2C2_BUS_VN 70
#define I2C3_SLAVE_VN 71
#define I2C3_MASTER_VN 72
#define I2C3_BUS_VN 73
#define CCP1_VN 74
#define CCT1_VN 75
#define CCP2_VN 76
#define CCT2_VN 77
#define CCP3_VN 78
#define CCT3_VN 79
#define CCP4_VN 80
#define CCT4_VN 81
#define CCP5_VN 82
#define CCT5_VN 83
#define CCP6_VN 84
#define CCT6_VN 85
#define CCP7_VN 86
#define CCT7_VN 87
#define CCP8_VN 88
#define CCT8_VN 89
#define CCP9_VN 90
#define CCT9_VN 91
#define FRC_TUNE_VN 92
#define NVM_VN 94
#define PERFORMANCE_COUNTER_VN 95
#define ECCSB_ERR_VN 97
#define DMA0_VN 98
#define DMA1_VN 99
#define DMA2_VN 100
#define DMA3_VN 101
////////////////////////////////////////////////////////////////////////////////
// Priority 0-7 (0=off, 7=highest)
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Type SRS, SOFT, AUTO (just use soft instead)
// just use these type names, no defines needed
// SOFT is default if not specified (instead of AUTO)
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// here are the macros (need to expand args, so ISR 'calls' _ISR)
////////////////////////////////////////////////////////////////////////////////
//the real work done here
#define _ISR(vn, nam, pri, typ, ...) \
    extern "C" {\
        void __attribute((vector(vn),interrupt(IPL##pri##typ))) nam##_ISR();\
    }\
    void nam##_ISR()

//we use this one
#define ISR(vn, pri, ...) _ISR(vn##_VN, vn, pri, ##__VA_ARGS__, AUTO)
////////////////////////////////////////////////////////////////////////////////
// description of macros
////////////////////////////////////////////////////////////////////////////////
/*

here is the 'manual' way to create an isr function (without macro help)

extern "C" {
void __attribute((vector(0), interrupt(IPL7SOFT))) CoreTimerISR(){
    Cp0::compare_reload();
    led2.invert();
    Irq::flagclear(Irq::CORE_TIMER);
}
}

 we first have to use 'extern "C" {}' to get the isr name into C namespace
 or else the linker cannot find it

 then the attributes to the function need to be defined-
 vector(0) will override the weak function __vector_dispatch_0, where a jump
 to our isr is placed instead of one to _DefaultInterrupt
 IPL7SOFT tells the compiler to create interrupt backup/restore registers code
 for the specified priority level and type

 NOTE- the priority level has to match the priority level previously set in
       other code for the specified irq, in this case the priority level is 7
       the SRS option can only be used if other code also sets this priority
       level to use the shadow register set


 the 'manual' way is not too hard, but is a little cumbersome with the
 attributes- we have to lookup the vector number when we lookup the name which
 can lead to using incorrect vector number, so defines/macros are used to help
 where we only need to come up with correct name


 here is what the same irq above looks like with the ISR macro-

 ISR(CORE_TIMER, 7, SOFT){
    Cp0::compare_reload();
    led2.invert();
    Irq::flagclear(Irq::CORE_TIMER);
 }

 or (using default of SOFT)

 ISR(CORE_TIMER, 7){
    Cp0::compare_reload();
    led2.invert();
    Irq::flagclear(Irq::CORE_TIMER);
 }

NOTE- the priority level has to match the priority level previously set in
      other code for the specified irq, in this case the priority level is 7
      the SRS option can only be used if other code also sets this priority
      level to use the shadow register set


the ISR names from Irq::IRQ_VN are used in the ISR macro (same as datasheet)
with _VN appended to keep defines from possibly colliding with others- in
the ISR macro, use the Irq::IRQ_VN name (which will not collide with same
names in Irq::IRQ_VN, since the enums are inside a class and are specified
by class when used)


NOTE- ## is macro string concatenation (except before __VA_ARGS__, which
      indicates we want the preceeding comma removed if empty)

first-
#define ISR(vn,pri,...) _ISR(vn##_VN,vn,pri,##__VA_ARGS__,SOFT)

 the ISR macro takes 3 arguments-
    vn is vector name
    pri is priority level 0-7
    ... is empty, SOFT, SRS, or AUTO

    something like  ISR(CORE_TIMER, 7, SOFT) or
                    ISR(CORE_TIMER, 7, SRS) or
                    ISR(CORE_TIMER, 7) default SOFT, as ... is empty

 the ISR then 'calls' _ISR with the arguments-
    vn##_VN, which is- CORE_TIMER_VN
    vn, which is CORE_TIMER
    pri, which is PRI7
    ..., which is SOFT  !UNLESS! specified
    (becomes SRS, SOFT  or SOFT  or SOFT, SOFT  or AUTO, SOFT)
    (the ##__VA_ARGS__ will be 'nothing' if no argument, which moves
     the default argument into the 'first' position of the following
     variadic... in _ISR)


second-
 _ISR(vn,nam,pri,typ,...) \
    extern "C" {\
        void __attribute((vector(vn),interrupt(IPL##pri##typ))) nam##_ISR();\
    }\
    void nam##_ISR()


 the _ISR macro runs with 'expanded' values
    CORE_TIMER_VN is now 0 (since we have a define with that name)
    CORE_TIMER is unchanged (we will use for name)
    7 is unchanged (priority)
    the typ is either the specified value, or SOFT
    (the ... is there to 'consume' the default argument if not used)

    so we now have _ISR(0,CORE_TIMER,7,SOFT)

 we first declare our function in the C namespace with the attributes set-
 vector(0),interrupt(IPL7SOFT), and the function name is CORE_TIMER_ISR
 (function name is not really important, except when looking at disassembled
  code we can see by the name what that function is for)

 now that the function is declared, we can get out of the "C" namespace and
 simply start the function definition with our declared name, we need to only
 add the opening brace, our code, and the closing brace

 simple.


*/