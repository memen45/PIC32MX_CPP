#pragma once

/*=============================================================================
 ADC
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Adc {

    public:

    //adc data format
    enum FORM {
        INT16 = 0<<8, SINT16 = 1<<8, FR16 = 2<<8, SFR16 = 3<<8,
        INT32 = 4<<8, SINT32 = 5<<8, FR32 = 6<<8, SFR32 = 7<<8
    };
    //trigger source select
    enum SSRC {
        SOFT = 0<<4, INT0 = 1<<4,
        TMR3 = 2<<4, TMR1 = 5<<4, TMR1SLEP = 6<<4,
        AUTO = 7<<4,
        MCCP1 = 8<<4, MCCP2 = 9<<4, MCCP3 = 10<<4,
        SCCP4 = 11<<4, SCCP5 = 12<<4, SCCP6 = 13<<4,
        CLC1 = 14<<4, CLC2 = 15<<4
    };
    //vref config
    enum VCFG {
        EXTP_EXTN = 3<<13, EXTP_VSS = 2<<13, VDD_EXTN = 1<<13, VDD_VSS = 0<<13
    };
    //auto-scan interrupt mode
    enum ASINT { NONE = 0<<8, DET = 1<<8, COMP = 2<<8, DETCOMP = 3<<8 };
    //write mode
    enum WM { LEGACY = 0<<2, CONVSAV = 1<<2, COMPONLY = 2<<2 };
    //compare mode
    enum CM { LT = 0, GT = 1, INWIN = 2, OUTWIN = 3 };
    //channel select
    enum CH0SA {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7, AN8, AN9, AN10, AN11,
        AN12,AN13, AN14, AN15, AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD,
        END = 255
    };
    //minimal conversion times for frc/pbclk(24MHz) for 10/12bits
    enum CONVTIME { FRC10BIT = 1, FRC12BIT, PBCLK10BIT, PBCLK12BIT };
    //clock source
    enum CLK { PBCLK = 0, FRC = 1 };

    //public functions
    //ADC1BUFn
    static uint16_t bufn            (uint8_t);
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

        
    enum {
        ADC1BUF0 = 0xBF802100, ADC1BUF_SPACING = 0x10, ADC1BUF_LAST = 21,
        ADC1CON1 = 0xBF802260,
            ON = 1<<15, SIDL = 1<<13,
            MODE12 = 1<<3, ASAM = 1<<2, SAMP = 1<<1, DONE = 1<<0,
        ADC1CON2 = 0xBF802270,
            OFFCAL = 1<<12, BUFREGEN = 1<<11, CSCNA = 1<<10, BUFS = 1<<7,
            SMPISHIFT = 2, SMPICLR = 7<<2, BUFM = 1<<0, VCFGCLR = 7<<13,
        ADC1CON3 = 0xBF802280,
            ADRC = 1<<15, EXTSAM = 1<<14, SAMCSHIFT = 8, SAMCCLR = 31<<8,
        ADC1CHS = 0xBF802290,
        ADC1CSS = 0xBF8022A0,
        ADC1CON5 = 0xBF8022C0,
            ASEN = 1<<15, LPEN = 1<<14, BGREQ = 1<<12, WMCLR = 3<<2,
        ADC1CHIT = 0xBF8022D0
    };
};


/*=============================================================================
 all functions inline
=============================================================================*/

//ADC1BUFn
uint16_t Adc::bufn(uint8_t n){
    if(n > ADC1BUF_LAST) n = ADC1BUF_LAST;
    return Reg::val16(ADC1BUF0+(n*ADC1BUF_SPACING));
}
//ADC1CON1
void Adc::on(bool tf){ Reg::set(ADC1CON1, ON, tf); }
void Adc::stop_idle(bool tf){ Reg::set(ADC1CON1, SIDL, tf); }
void Adc::format(FORM e){ Reg::clr(ADC1CON1, SFR32); Reg::set(ADC1CON1, e); }
void Adc::trig_sel(SSRC e){ Reg::clr(ADC1CON1, CLC2); Reg::set(ADC1CON1, e); }
void Adc::mode_12bit(bool tf){ Reg::set(ADC1CON1, MODE12, tf); }
void Adc::samp_auto(bool tf){ Reg::set(ADC1CON1, ASAM, tf); }
void Adc::samp(bool tf){ Reg::set(ADC1CON1, SAMP, tf); }
bool Adc::samp(){ return Reg::is_set(ADC1CON1, SAMP); }
bool Adc::done(){ return Reg::is_set(ADC1CON1, DONE); }
//ADC1CON2
void Adc::vref_cfg(VCFG e){
    Reg::clr(ADC1CON2, VCFGCLR);
    Reg::set(ADC1CON2, e);
}
void Adc::offset_cal(bool tf){ Reg::set(ADC1CON2, OFFCAL, tf); }
void Adc::buf_reg(bool tf){ Reg::set(ADC1CON2, BUFREGEN, tf); }
void Adc::scan(bool tf){ Reg::set(ADC1CON2, CSCNA, tf); }
bool Adc::buf2nd_busy(){ Reg::is_set(ADC1CON2, BUFS); }
void Adc::samp_nirq(uint8_t n){
    n -= 1; n &= 15; //n = 1-16
    Reg::clr(ADC1CON2, SMPICLR);
    Reg::set(ADC1CON2, n<<SMPISHIFT);
}
void Adc::buf_split(bool tf){ Reg::set(ADC1CON2, BUFM, tf); }
//ADC1CON3
void Adc::clk_src(CLK e){ Reg::set(ADC1CON3, ADRC, e); }
void Adc::samp_extend(bool tf){ Reg::set(ADC1CON3, EXTSAM, tf); }
void Adc::samp_time(uint8_t t){
    t &= 31; t = t == 0 ? 1 : t; //0 not allowed (1-31)
    Reg::clr(ADC1CON3, SAMCCLR);
    Reg::set(ADC1CON3, t<<SAMCSHIFT);
}
//default value is for 24MHz, 4 will meet 280ns Tad for any clock
void Adc::conv_time(uint8_t t){ Reg::val8(ADC1CON3, t); }
//ADC1CON5
void Adc::scan_auto(bool tf){ Reg::set(ADC1CON5, ASEN, tf); }
void Adc::low_power(bool tf){ Reg::set(ADC1CON5, LPEN, tf); }
void Adc::bandgap(bool tf){ Reg::set(ADC1CON5, BGREQ, tf); }
void Adc::scan_autoirq(ASINT e){
    Reg::clr(ADC1CON5, DETCOMP);
    Reg::set(ADC1CON5, e);
}
void Adc::write_mode(WM e){ Reg::clr(ADC1CON5, WMCLR); Reg::set(ADC1CON5, e); }
void Adc::compare_mode(CM e){
    Reg::clr(ADC1CON5, OUTWIN);
    Reg::set(ADC1CON5, e);
}
//ADC1CHS
void Adc::ch_sel(CH0SA e){ Reg::val8(ADC1CHS, e);}
//ADC1SS
void Adc::ch_scan(CH0SA e, bool tf){ Reg::set(ADC1CSS, e, tf); }
void Adc::ch_scan(CH0SA* e){
    Reg::val(ADC1CSS, 0); //clr all
    for(; *e != END; e++) Reg::set(ADC1CSS, 1<<*e); //set list
}
void Adc::ch_scan(uint32_t v){ Reg::val(ADC1CSS, v); }
//ADC1CHIT
//non AN values will return 0 (like VDD)
bool Adc::ch_hit(CH0SA e){ return Reg::is_set(ADC1CHIT, 1<<e); }
uint32_t Adc::ch_hit(){ return Reg::val(ADC1CHIT); }

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