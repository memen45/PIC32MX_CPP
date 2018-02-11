#pragma once

//ADC

#include <cstdint>

struct Adc {

    //ADC1BUFn
    static uint16_t     read            (uint8_t);
    static uint16_t     read            ();

    //ADC1CON1
    static void         on              (bool);
    static void         stop_idle       (bool);

    enum FORM : uint8_t {
        INT16 = 0, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
    };
    static void         format          (FORM);

    enum SSRC : uint8_t {
        SOFT = 0, INT0, TMR3, TMR1 = 5, TMR1SLEP, AUTO, MCCP1, MCCP2, MCCP3,
        SCCP4, SCCP5, SCCP6, CLC1, CLC2
    };
    static void         trig_sel        (SSRC);

    static void         mode_12bit      (bool);
    static void         samp_auto       (bool);
    static void         samp            (bool);
    static bool         samp            ();
    static bool         done            ();

    //ADC1CON2
    enum VCFG :uint8_t {
        VDD_VSS = 0, VDD_EXTN, EXTP_VSS, EXTP_EXTN
    };
    static void         vref_cfg        (VCFG);

    static void         offset_cal      (bool);
    static void         buf_reg         (bool);
    static void         scan            (bool);
    static bool         buf2nd_busy     ();
    static void         samp_nirq       (uint8_t);
    static void         buf_split       (bool);

    //ADC1CON3
    enum CLK { PBCLK = 0, FRC };
    static void         clk_src         (CLK);

    static void         samp_extend     (bool);
    static void         samp_time       (uint8_t);
    static void         conv_time       (uint8_t = 4);

    //ADC1CON5
    static void         scan_auto       (bool);
    static void         low_power       (bool);
    static void         bandgap         (bool);

    enum ASINT : uint8_t { NONE = 0, DET, COMP, DETCOMP };
    static void         scan_autoirq    (ASINT);

    enum WM : uint8_t { LEGACY = 0, CONVSAV, COMPONLY };
    static void         write_mode      (WM);

    enum CM :uint8_t { LT = 0, GT, INWIN, OUTWIN };
    static void         compare_mode    (CM);

    //ADC1CHS
    enum CH0SA : uint8_t {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7, AN8, AN9, AN10, AN11,
        AN12,AN13, AN14, AN15, AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD,
        END = 255
    };
    static void         ch_sel          (CH0SA);
    static void         ch_sel          (uint8_t);

    //ADC1SS
    static void         ch_scan         (CH0SA, bool);
    static void         ch_scan         (CH0SA*);

    static void         ch_scan         (uint32_t);

    //ADC1CHIT
    static bool         ch_hit          (CH0SA);
    static uint32_t     ch_hit          ();

    //minimal conversion times for frc/pbclk(24MHz) for 10/12bits
    enum CONVTIME { FRC10BIT = 1, FRC12BIT, PBCLK10BIT, PBCLK12BIT };
};


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
