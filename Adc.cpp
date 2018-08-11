#include "Adc.hpp"
#include "Reg.hpp"

enum {
ADC1BUF_SPACING = 0x10,
ADC1BUF_LAST = 21,
ADC1BUF0 = 0xBF802100,
ADC1CON1 = 0xBF802260,
    ON = 15,
    SIDL = 13,
    FORM_SHIFT = 8, FORM_MASK = 7,
    SSRC_SHIFT = 4, SSRC_MASK = 15,
    MODE12 = 3,
    ASAM = 2,
    SAMP = 1,
    DONE = 0,
ADC1CON2 = 0xBF802270,
    VCFG_SHIFT = 13, VCFG_MASK = 7,
    OFFCAL = 12,
    BUFREGEN = 11,
    CSCNA = 10,
    BUFS = 7,
    SMPI_SHIFT = 2, SMPI_MASK = 7,
    BUFM = 1,
ADC1CON3 = 0xBF802280,
    ADRC = 15,
    EXTSAM = 14,
    SAMC_SHIFT = 8, SAMC_MASK = 31,
ADC1CHS = 0xBF802290,
ADC1CSS = 0xBF8022A0,
ADC1CON5 = 0xBF8022C0,
    ASEN = 15,
    LPEN = 14,
    BGREQ = 12,
    ASINT_SHIFT = 8, ASINT_MASK = 3,
    WM_SHIFT = 2, WM_MASK = 3,
    CM_SHIFT = 0, CM_MASK = 3,
ADC1CHIT = 0xBF8022D0
};

//ADC1BUFn
//=============================================================================
            auto Adc::
read        (uint8_t n) -> uint32_t
            {
            if(n > ADC1BUF_LAST) n = ADC1BUF_LAST;
            return Reg::val(ADC1BUF0 + (n * ADC1BUF_SPACING));
            }

//ADC1CON1
//=============================================================================
            auto Adc::
on          (bool tf) -> void
            {
            Reg::setbit(ADC1CON1, 1<<ON, tf);
            }

//=============================================================================
            auto Adc::
stop_idle   (bool tf) -> void
            {
            Reg::setbit(ADC1CON1, 1<<SIDL, tf);
            }

//=============================================================================
            auto Adc::
format      (FORM e) -> void
            {
            Reg::clrbit(ADC1CON1, FORM_MASK<<FORM_SHIFT);
            Reg::setbit(ADC1CON1, e<<FORM_SHIFT);
            }

//=============================================================================
            auto Adc::
trig_sel    (SSRC e) -> void
            {
            Reg::clrbit(ADC1CON1, SSRC_MASK<<SSRC_SHIFT);
            Reg::setbit(ADC1CON1, e<<SSRC_SHIFT);
            }

//=============================================================================
            auto Adc::
mode_12bit  (bool tf) -> void
            {
            Reg::setbit(ADC1CON1, 1<<MODE12, tf);
            }

//=============================================================================
            auto Adc::
samp_auto   (bool tf) -> void
            {
            Reg::setbit(ADC1CON1, 1<<ASAM, tf);
            }

//=============================================================================
            auto Adc::
samp        (bool tf) -> void
            {
            Reg::setbit(ADC1CON1, 1<<SAMP, tf);
            }

//=============================================================================
            auto Adc::
samp        () -> bool
            {
            return Reg::anybit(ADC1CON1, 1<<SAMP);
            }

//=============================================================================
            auto Adc::
done        () -> bool
            {
            return Reg::anybit(ADC1CON1, 1<<DONE);
            }

//ADC1CON2
//=============================================================================
            auto Adc::
vref_cfg    (VCFG e) -> void
            {
            Reg::clrbit(ADC1CON2, VCFG_MASK<<VCFG_SHIFT);
            Reg::setbit(ADC1CON2, e<<VCFG_SHIFT);
            }

//=============================================================================
            auto Adc::
offset_cal  (bool tf) -> void
            {
            Reg::setbit(ADC1CON2, 1<<OFFCAL, tf);
            }

//=============================================================================
            auto Adc::
buf_reg     (bool tf) -> void
            {
            Reg::setbit(ADC1CON2, 1<<BUFREGEN, tf);
            }

//=============================================================================
            auto Adc::
scan        (bool tf) -> void
            {
            Reg::setbit(ADC1CON2, 1<<CSCNA, tf);
            }

//=============================================================================
            auto Adc::
buf2nd_busy () -> bool
            {
            return Reg::anybit(ADC1CON2, 1<<BUFS);
            }

//=============================================================================
            auto Adc::
samp_nirq   (uint8_t n) -> void
            {
            n -= 1; n and_eq 15; //n = 1-16 ->0-15
            Reg::clrbit(ADC1CON2, SMPI_MASK<<SMPI_SHIFT);
            Reg::setbit(ADC1CON2, n<<SMPI_SHIFT);
            }

//=============================================================================
            auto Adc::
buf_split   (bool tf) -> void
            {
            Reg::setbit(ADC1CON2, 1<<BUFM, tf);
            }

//ADC1CON3
//=============================================================================
            auto Adc::
clk_src     (CLK e) -> void
            {
            Reg::setbit(ADC1CON3, 1<<ADRC, e);
            }

//=============================================================================
            auto Adc::
samp_extend (bool tf) -> void
            {
            Reg::setbit(ADC1CON3, 1<<EXTSAM, tf);
            }

//=============================================================================
            auto Adc::
samp_time   (uint8_t v) -> void
            {
            v and_eq 31; v = v == 0 ? 1 : v; //0 not allowed (1-31)
            Reg::clrbit(ADC1CON3, SAMC_MASK<<SAMC_SHIFT);
            Reg::setbit(ADC1CON3, v<<SAMC_SHIFT);
            }

//default value is for 24MHz, 4 will meet 280ns Tad for any clock
//=============================================================================
            auto Adc::
conv_time   (uint8_t v) -> void
            {
            Reg::val(ADC1CON3, v);
            }

//ADC1CON5
//=============================================================================
            auto Adc::
scan_auto   (bool tf) -> void
            {
            Reg::setbit(ADC1CON5, 1<<ASEN, tf);
            }

//=============================================================================
            auto Adc::
low_power   (bool tf) -> void
            {
            Reg::setbit(ADC1CON5, 1<<LPEN, tf);
            }

//=============================================================================
            auto Adc::
bandgap     (bool tf) -> void
            {
            Reg::setbit(ADC1CON5, 1<<BGREQ, tf);
            }

//=============================================================================
            auto Adc::
scan_autoirq (ASINT e) -> void
            {
            Reg::clrbit(ADC1CON5, ASINT_MASK<<ASINT_SHIFT);
            Reg::setbit(ADC1CON5, e<<ASINT_SHIFT);
            }

//=============================================================================
            auto Adc::
write_mode  (WM e) -> void
            {
            Reg::clrbit(ADC1CON5, WM_MASK<<WM_SHIFT);
            Reg::setbit(ADC1CON5, e<<WM_SHIFT);
            }

//=============================================================================
            auto Adc::
compare_mode (CM e) -> void
            {
            Reg::clrbit(ADC1CON5, CM_MASK<<CM_SHIFT);
            Reg::setbit(ADC1CON5, e<<CM_SHIFT);
            }

//ADC1CHS
//=============================================================================
            auto Adc::
ch_sel      (CH0SA e) -> void
            {
            Reg::val(ADC1CHS, e);
            }

//ADC1SS
//single channel on/off
//=============================================================================
            auto Adc::
ch_scan     (CH0SA e, bool tf) -> void
            {
            Reg::setbit(ADC1CSS, 1<<e, tf);
            }

//multiple channel on, from an array of CH0SA (terminated by END)
//=============================================================================
            auto Adc::
ch_scan     (CH0SA* e) -> void
            {
            Reg::val(ADC1CSS, 0); //clr all
            for(; *e not_eq END; e++) Reg::setbit(ADC1CSS, 1<<*e); //set list
            }

//just set all bits as needed manually in one shot
//=============================================================================
            auto Adc::
ch_scan     (uint32_t v) -> void
            {
            Reg::val(ADC1CSS, v);
            }

//ADC1CHIT
//non AN values will return 0 (like VDD)
//=============================================================================
            auto Adc::
ch_hit      (CH0SA e) -> bool
            {
            return Reg::anybit(ADC1CHIT, 1<<e);
            }

//=============================================================================
            auto Adc::
ch_hit      () -> uint32_t
            {
            return Reg::val(ADC1CHIT);
            }
