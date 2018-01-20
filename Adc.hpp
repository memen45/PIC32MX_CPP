#pragma once

//ADC

#include <cstdint>
#include "Reg.hpp"

struct Adc {

    //adc data format
    enum FORM : uint8_t {
        INT16 = 0, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
    };

    //trigger source select
    enum SSRC : uint8_t {
        SOFT = 0, INT0, TMR3, TMR1 = 5, TMR1SLEP, AUTO, MCCP1, MCCP2, MCCP3,
        SCCP4, SCCP5, SCCP6, CLC1, CLC2
    };

    //vref config
    enum VCFG :uint8_t {
        VDD_VSS = 0, VDD_EXTN, EXTP_VSS, EXTP_EXTN
    };

    //auto-scan interrupt mode
    enum ASINT : uint8_t { NONE = 0, DET, COMP, DETCOMP };

    //write mode
    enum WM : uint8_t { LEGACY = 0, CONVSAV, COMPONLY };

    //compare mode
    enum CM :uint8_t { LT = 0, GT, INWIN, OUTWIN };

    //channel select
    enum CH0SA : uint8_t {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7, AN8, AN9, AN10, AN11,
        AN12,AN13, AN14, AN15, AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD,
        END = 255
    };

    //minimal conversion times for frc/pbclk(24MHz) for 10/12bits
    enum CONVTIME { FRC10BIT = 1, FRC12BIT, PBCLK10BIT, PBCLK12BIT };

    //clock source
    enum CLK { PBCLK = 0, FRC };

    //ADC1BUFn
    static uint16_t     bufn            (uint8_t);

    //ADC1CON1
    static void         on              (bool);
    static void         stop_idle       (bool);
    static void         format          (FORM);
    static void         trig_sel        (SSRC);
    static void         mode_12bit      (bool);
    static void         samp_auto       (bool);
    static void         samp            (bool);
    static bool         samp            ();
    static bool         done            ();

    //ADC1CON2
    static void         vref_cfg        (VCFG);
    static void         offset_cal      (bool);
    static void         buf_reg         (bool);
    static void         scan            (bool);
    static bool         buf2nd_busy     ();
    static void         samp_nirq       (uint8_t);
    static void         buf_split       (bool);

    //ADC1CON3
    static void         clk_src         (CLK);
    static void         samp_extend     (bool);
    static void         samp_time       (uint8_t);
    static void         conv_time       (uint8_t = 4);

    //ADC1CON5
    static void         scan_auto       (bool);
    static void         low_power       (bool);
    static void         bandgap         (bool);
    static void         scan_autoirq    (ASINT);
    static void         write_mode      (WM);
    static void         compare_mode    (CM);

    //ADC1CHS
    static void         ch_sel          (CH0SA);

    //ADC1SS
    static void         ch_scan         (CH0SA, bool);
    static void         ch_scan         (CH0SA*);
    static void         ch_scan         (uint32_t);

    //ADC1CHIT
    static bool         ch_hit          (CH0SA);
    static uint32_t     ch_hit          ();

    private:

    static Reg r;

    enum {
        ADC1BUF0 = 0xBF802100, ADC1BUF_SPACING = 0x10, ADC1BUF_LAST = 21,
        ADC1CON1 = 0xBF802260,
            ON = 1<<15,
            SIDL = 1<<13,
            FORM_SHIFT = 8, FORM_CLR = 7,
            SSRC_SHIFT = 4, SSRC_CLR = 15,
            MODE12 = 1<<3,
            ASAM = 1<<2,
            SAMP = 1<<1,
            DONE = 1<<0,
        ADC1CON2 = 0xBF802270,
            VCFG_SHIFT = 13, VCFG_CLR = 7,
            OFFCAL = 1<<12,
            BUFREGEN = 1<<11,
            CSCNA = 1<<10,
            BUFS = 1<<7,
            SMPI_SHIFT = 2, SMPI_CLR = 7,
            BUFM = 1<<1,
        ADC1CON3 = 0xBF802280,
            ADRC = 1<<15,
            EXTSAM = 1<<14,
            SAMC_SHIFT = 8, SAMC_CLR = 31,
        ADC1CHS = 0xBF802290,
        ADC1CSS = 0xBF8022A0,
        ADC1CON5 = 0xBF8022C0,
            ASEN = 1<<15,
            LPEN = 1<<14,
            BGREQ = 1<<12,
            ASINT_SHIFT = 8, ASINT_CLR = 3,
            WM_SHIFT = 2, WM_CLR = 3,
            CM_SHIFT = 0, CM_CLR = 3,
        ADC1CHIT = 0xBF8022D0
    };

};

//ADC1BUFn
//=============================================================================
    uint16_t        Adc::bufn           (uint8_t n)
//=============================================================================
{
    if(n > ADC1BUF_LAST) n = ADC1BUF_LAST;
    return r.val16(ADC1BUF0+(n*ADC1BUF_SPACING));
}

//ADC1CON1
//=============================================================================
    void            Adc::on             (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON1, ON, tf);
}

//=============================================================================
    void            Adc::stop_idle      (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON1, SIDL, tf);
}

//=============================================================================
    void            Adc::format         (FORM e)
//=============================================================================
{
    r.clrbit(ADC1CON1, FORM_CLR<<FORM_SHIFT);
    r.setbit(ADC1CON1, e<<FORM_SHIFT);
}

//=============================================================================
    void            Adc::trig_sel       (SSRC e)
//=============================================================================
{
    r.clrbit(ADC1CON1, SSRC_CLR<<SSRC_SHIFT);
    r.setbit(ADC1CON1, e<<SSRC_SHIFT);
}

//=============================================================================
    void            Adc::mode_12bit     (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON1, MODE12, tf);
}

//=============================================================================
    void            Adc::samp_auto      (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON1, ASAM, tf);
}

//=============================================================================
    void            Adc::samp           (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON1, SAMP, tf);
}

//=============================================================================
    bool            Adc::samp           ()
//=============================================================================
{
    return r.anybit(ADC1CON1, SAMP);
}

//=============================================================================
    bool            Adc::done           ()
//=============================================================================
{
    return r.anybit(ADC1CON1, DONE);
}

//ADC1CON2
//=============================================================================
    void            Adc::vref_cfg       (VCFG e)
//=============================================================================
{
    r.clrbit(ADC1CON2, VCFG_CLR<<VCFG_SHIFT);
    r.setbit(ADC1CON2, e<<VCFG_SHIFT);
}

//=============================================================================
    void            Adc::offset_cal     (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON2, OFFCAL, tf);
}

//=============================================================================
    void            Adc::buf_reg        (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON2, BUFREGEN, tf);
}

//=============================================================================
    void            Adc::scan           (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON2, CSCNA, tf);
}

//=============================================================================
    bool            Adc::buf2nd_busy    ()
//=============================================================================
{
    return r.anybit(ADC1CON2, BUFS);
}

//=============================================================================
    void            Adc::samp_nirq      (uint8_t n)
//=============================================================================
{
    n -= 1; n &= 15; //n = 1-16 ->0-15
    r.clrbit(ADC1CON2, SMPI_CLR<<SMPI_SHIFT);
    r.setbit(ADC1CON2, n<<SMPI_SHIFT);
}

//=============================================================================
    void            Adc::buf_split      (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON2, BUFM, tf);
}

//ADC1CON3
//=============================================================================
    void            Adc::clk_src        (CLK e)
//=============================================================================
{
    r.setbit(ADC1CON3, ADRC, e);
}

//=============================================================================
    void            Adc::samp_extend    (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON3, EXTSAM, tf);
}

//=============================================================================
    void            Adc::samp_time      (uint8_t v)
//=============================================================================
{
    v &= 31; v = v == 0 ? 1 : v; //0 not allowed (1-31)
    r.clrbit(ADC1CON3, SAMC_CLR<<SAMC_SHIFT);
    r.setbit(ADC1CON3, v<<SAMC_SHIFT);
}

//default value is for 24MHz, 4 will meet 280ns Tad for any clock
//=============================================================================
    void            Adc::conv_time      (uint8_t v)
//=============================================================================
{
    r.val(ADC1CON3, v);
}

//ADC1CON5
//=============================================================================
    void            Adc::scan_auto      (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON5, ASEN, tf);
}

//=============================================================================
    void            Adc::low_power      (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON5, LPEN, tf);
}

//=============================================================================
    void            Adc::bandgap        (bool tf)
//=============================================================================
{
    r.setbit(ADC1CON5, BGREQ, tf);
}

//=============================================================================
    void            Adc::scan_autoirq   (ASINT e)
//=============================================================================
{
    r.clrbit(ADC1CON5, ASINT_CLR<<ASINT_SHIFT);
    r.setbit(ADC1CON5, e<<ASINT_SHIFT);
}

//=============================================================================
    void            Adc::write_mode     (WM e)
//=============================================================================
{
    r.clrbit(ADC1CON5, WM_CLR<<WM_SHIFT);
    r.setbit(ADC1CON5, e<<WM_SHIFT);
}

//=============================================================================
    void            Adc::compare_mode   (CM e)
//=============================================================================
{
    r.clrbit(ADC1CON5, CM_CLR<<CM_SHIFT);
    r.setbit(ADC1CON5, e<<CM_SHIFT);
}

//ADC1CHS
//=============================================================================
    void            Adc::ch_sel         (CH0SA e)
//=============================================================================
{
    r.val(ADC1CHS, e);
}

//ADC1SS
//=============================================================================
    void            Adc::ch_scan        (CH0SA e, bool tf)
//=============================================================================
{
    r.setbit(ADC1CSS, e, tf);
}

//=============================================================================
    void            Adc::ch_scan        (CH0SA* e)
//=============================================================================
{
    r.val(ADC1CSS, 0); //clr all
    for(; *e != END; e++) r.setbit(ADC1CSS, 1<<*e); //set list
}

//=============================================================================
    void            Adc::ch_scan        (uint32_t v)
//=============================================================================
{
    r.val(ADC1CSS, v);
}

//ADC1CHIT
//non AN values will return 0 (like VDD)
//=============================================================================
    bool            Adc::ch_hit         (CH0SA e)
//=============================================================================
{
    return r.anybit(ADC1CHIT, 1<<e);
}

//=============================================================================
    uint32_t        Adc::ch_hit         ()
//=============================================================================
{
    return r.val(ADC1CHIT);
}



/*
 misc info

12bit mode
Tad     ADC Clock Period    280ns
Tconv   Comversion Time     14Tad
Tpss    Sample Start Delay  1-3Tad
Fcnv    Throughput          200ksps

10bit mode
Tad     ADC Clock Period    200ns
Tconv   Conversion Time     12Tad
Tpss    Sample Start Delay  1-3Tad
Fcnv    Throughput          300ksps


Tsrc    FastRC 8Mhz         1/8MHz  = 125ns
Tsrc    PBclk 24MHz         1/24MHz = 41.6ns

Tad =   adcs<7:0> * 2 * Tsrc
        (adcs=0, then Tad=Tsrc)

Tsrc = 8MHz (Frc)
adcs = 0 -> 1 * 125ns = 125ns
adcs = 1 -> 1 * 2 * 125ns = 250ns <-min use for 10bit Frc
adcs = 2 -> 2 * 2 * 125ns = 500ns <-min use for 12bit Frc
adcs = 3 -> 3 * 2 * 125ns = 750ns
adcs = 4 -> 4 * 2 * 125ns = 1000ns
adcs = 5 -> 5 * 2 * 125ns = 1250ns
adcs = 6 -> 6 * 2 * 125ns = 1500ns
adcs = 255 -> 255 * 2 * 125ns = 63.750us

Tsrc = 24MHz (PBclk max)
adcs = 0 -> 1 * 41.6ns = 41.6ns
adcs = 1 -> 1 * 2 * 41.6ns = 83ns
adcs = 2 -> 2 * 2 * 41.6ns = 166ns
adcs = 3 -> 3 * 2 * 41.6ns = 250ns <-min use for 10bit 24MHz
adcs = 4 -> 4 * 2 * 41.6ns = 333ns <-min use for 12bit 24MHz
adcs = 5 -> 5 * 2 * 41.6ns = 416ns
adcs = 6 -> 6 * 2 * 41.6ns = 500ns
adcs = 255 -> 255 * 2 * 41.6ns = 21.25us

*/