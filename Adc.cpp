#include "Adc.hpp"
#include "Reg.hpp"

enum {
    ADC1BUF_SPACING = 0x10,
    ADC1BUF_LAST = 15,
    ADC1BUF0 = 0xBF809070,
    ADC1CON1 = 0xBF809000,
        ON = 15,
        SIDL = 13,
        FORM_SHIFT = 8, FORM_MASK = 7,
        SSRC_SHIFT = 5, SSRC_MASK = 7,
		CLRASAM = 4,
        ASAM = 2,
        SAMP = 1,
        DONE = 0,
    ADC1CON2 = 0xBF809010,
        VCFG_SHIFT = 13, VCFG_MASK = 7,
        OFFCAL = 12,
        CSCNA = 10,
        BUFS = 7,
        SMPI_SHIFT = 2, SMPI_MASK = 15,
        BUFM = 1,
		ALTS = 0,
    ADC1CON3 = 0xBF809020,
        ADRC = 15,
        SAMC_SHIFT = 8, SAMC_MASK = 31,
    ADC1CHS = 0xBF809040,
        CH0NB = 31,
		CH0NA = 23,
		CH0SA_SHIFT = 16, CH0SB_SHIFT = 24, CH0S_MASK = 15,
		
    ADC1CSS = 0xBF809050
};

//ADC1BUFn
//=============================================================================
            auto Adc::
read        (BUFN n) -> uint32_t
            {
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
clr_asam    (bool tf) -> void
            {
            Reg::setbit(ADC1CON1, 1<<CLRASAM, tf);
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

//=============================================================================
            auto Adc::
alt_input   (bool tf) -> void
            {
            Reg::setbit(ADC1CON2, 1<<ALTS, tf);
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
samp_time   (uint8_t v) -> void
            {
            v and_eq 31; v = v == 0 ? 1 : v; //0 not allowed (1-31)
            Reg::clrbit(ADC1CON3, SAMC_MASK<<SAMC_SHIFT);
            Reg::setbit(ADC1CON3, v<<SAMC_SHIFT);
            }

//default value is for 80MHz, 7 will meet 200ns Tad for any clock
//=============================================================================
            auto Adc::
conv_time   (uint8_t v) -> void
            {
            Reg::val(ADC1CON3, v);
            }

//ADC1CHS
//=============================================================================
            auto Adc::
ch_sel      (CH0S pos, CH0N neg, SAMPLE sam) -> void
            {
            Reg::val(ADC1CHS, (pos << (sam ? CH0SB_SHIFT : CH0SA_SHIFT)) bitor 
                                        (neg << sam ? CH0NB : CH0NA));
            }

//ADC1SS
//single channel on/off
//=============================================================================
            auto Adc::
ch_scan     (CH0S e, bool tf) -> void
            {
            Reg::setbit(ADC1CSS, 1<<e, tf);
            }

//multiple channel on, from an array of CH0SA (terminated by END)
//=============================================================================
            auto Adc::
ch_scan     (CH0S* e) -> void
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
