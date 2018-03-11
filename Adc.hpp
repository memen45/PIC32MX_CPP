#pragma once

//ADC

#include <cstdint>

struct Adc {

//ADC1BUFn

    //read adc buffer N , N = 0 - ADC1BUF_LAST (21) , N default = 0
    static uint16_t     read            (uint8_t = 0);

//ADC1CON1

    //enable adc
    static void         on              (bool);

    //enable stop adc when in idle mode
    static void         stop_idle       (bool);

    //adc data output format
    enum FORM : uint8_t {
        INT16, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
    };
    static void         format          (FORM);

    //adc conversion trigger source select
    enum SSRC : uint8_t {
        SOFT = 0, INT0, TMR3, TMR1 = 5, TMR1SLEP, AUTO,
        MCCP1, MCCP2, MCCP3, SCCP4, SCCP5, SCCP6, CLC1, CLC2
    };
    static void         trig_sel        (SSRC);

    //true = 12bit conversion, false = 10bit conversion
    static void         mode_12bit      (bool);

    //adc sample auto start, false = wait for samp bit set
    static void         samp_auto       (bool);

    //true = adc sample enable, false = end sample, start conversion
    static void         samp            (bool);

    //get adc sample status
    static bool         samp            ();

    //get adc conversion status, true = done
    static bool         done            ();

//ADC1CON2

    //set Vr+/Vr- adc voltage reference
    enum VCFG : uint8_t {
        VDD_VSS, VDD_EXTN, EXTP_VSS, EXTP_EXTN
    };
    static void         vref_cfg        (VCFG);

    //connect SHA inputs to negative reference for offset calibration
    static void         offset_cal      (bool);

    //true = store N channel adc into buffer N, false = FIFO
    static void         buf_reg         (bool);

    //enable channel scan
    static void         scan            (bool);

    //true = second half of buffers busy, false = first half busy
    static bool         buf2nd_busy     ();

    //number of samples per irq, 1-16
    static void         samp_nirq       (uint8_t);

    //split buffer into two halves
    static void         buf_split       (bool);

//ADC1CON3

    //set adc conversion clock source
    enum CLK { PBCLK, FRC };
    static void         clk_src         (CLK);

    //keep sampling even when samp=0
    static void         samp_extend     (bool);

    //auto sample time, 1-31 Tad
    static void         samp_time       (uint8_t);

    //set conversion clock time Tad, default = 4 (max needed)
    static void         conv_time       (uint8_t = 4);

//ADC1CON5

    //auto scan enable
    static void         scan_auto       (bool);

    //enable low power after scan
    static void         low_power       (bool);

    //enable bandgap when adc on
    static void         bandgap         (bool);

    //auto scan interrupt modes
    enum ASINT : uint8_t { NONE, DET, COMP, DETCOMP };
    static void         scan_autoirq    (ASINT);

    //auto scan write mode
    enum WM : uint8_t { LEGACY, CONVSAV, COMPONLY };
    static void         write_mode      (WM);

    //auto scan compare mode
    enum CM :uint8_t { LT, GT, INWIN, OUTWIN };
    static void         compare_mode    (CM);

//ADC1CHS

    //adc input channel select
    enum CH0SA : uint8_t {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7,
        AN8, AN9, AN10, AN11, AN12,AN13, AN14, AN15,
        AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD, END = 255
    };
    static void         ch_sel          (CH0SA);

//ADC1SS

    //enable adc channel scan input, one channel
    static void         ch_scan         (CH0SA, bool);

    //enable adc channel scan input, one or more channels (in array)
    static void         ch_scan         (CH0SA*);

    //enable adc channel scan input, manually specify channels
    static void         ch_scan         (uint32_t);

//ADC1CHIT

    //get channel adc compare hit bit
    static bool         ch_hit          (CH0SA);

    //get all channels adc compare hit bit
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
