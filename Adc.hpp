#pragma once

//ADC

#include <cstdint>

struct Adc {

    //ADC1BUFn

    //read adc buffer N , N = 0 - ADC1BUF_LAST (21) , N default = 0
    static auto
    read (uint8_t = 0) -> uint16_t;

    //ADC1CON1

    //enable adc
    static auto
    on (bool) -> void;

    //enable stop adc when in idle mode
    static auto
    stop_idle (bool) -> void;

    //adc data output format
    enum FORM : uint8_t {
        INT16, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
    };
    static auto
    format (FORM) -> void;

    //adc conversion trigger source select
    enum SSRC : uint8_t {
        SOFT = 0, INT0, TMR3, TMR1 = 5, TMR1SLEP, AUTO,
        MCCP1, MCCP2, MCCP3, SCCP4, SCCP5, SCCP6, CLC1, CLC2
    };
    static auto
    trig_sel (SSRC) -> void;

    //true = 12bit conversion, false = 10bit conversion
    static auto
    mode_12bit (bool) -> void;

    //adc sample auto start, false = wait for samp bit set
    static auto
	samp_auto (bool) -> void;

    //true = adc sample enable, false = end sample, start conversion
    static auto
    samp (bool) -> void;

    //get adc sample status
    static auto
    samp () -> bool;

    //get adc conversion status, true = done
    static auto
    done () -> bool;

    //ADC1CON2

    //set Vr+/Vr- adc voltage reference
    enum VCFG : uint8_t {
        VDD_VSS, VDD_EXTN, EXTP_VSS, EXTP_EXTN
    };
    static auto
	vref_cfg (VCFG) -> void;

    //connect SHA inputs to negative reference for offset calibration
    static auto
    offset_cal (bool) -> void;

    //true = store N channel adc into buffer N, false = FIFO
    static auto
	buf_reg (bool) -> void;

    //enable channel scan
    static auto
	scan (bool) -> void;

    //true = second half of buffers busy, false = first half busy
    static auto
    buf2nd_busy () -> bool;

    //number of samples per irq, 1-16
    static auto
	samp_nirq (uint8_t) -> void;

    //split buffer into two halves
    static auto
	buf_split (bool) -> void;

    //ADC1CON3

    //set adc conversion clock source
    enum CLK { PBCLK, FRC };
    static auto
    clk_src (CLK) -> void;

    //keep sampling even when samp=0
    static auto
    samp_extend (bool) -> void;

    //auto sample time, 1-31 Tad
    static auto
    samp_time (uint8_t) -> void;

    //set conversion clock time Tad, default = 4 (max needed)
    static auto
	conv_time (uint8_t = 4) -> void;

    //ADC1CON5

    //auto scan enable
    static auto
    scan_auto (bool) -> void;

    //enable low power after scan
    static auto
    low_power (bool) -> void;

    //enable bandgap when adc on
    static auto
	bandgap (bool) -> void;

    //auto scan interrupt modes
    enum ASINT : uint8_t { NONE, DET, COMP, DETCOMP };
    static auto
	scan_autoirq (ASINT) -> void;

    //auto scan write mode
    enum WM : uint8_t { LEGACY, CONVSAV, COMPONLY };
    static auto
	write_mode (WM) -> void;

    //auto scan compare mode
    enum CM :uint8_t { LT, GT, INWIN, OUTWIN };
    static auto
	compare_mode (CM) -> void;

    //ADC1CHS

    //adc input channel select
    enum CH0SA : uint8_t {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7,
        AN8, AN9, AN10, AN11, AN12,AN13, AN14, AN15,
        AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD, END = 255
    };
    static auto
	ch_sel (CH0SA) -> void;

    //ADC1SS

    //enable adc channel scan input, one channel
    static auto
    ch_scan (CH0SA, bool) -> void;

    //enable adc channel scan input, one or more channels (in array)
    static auto
    ch_scan (CH0SA*) -> void;

    //enable adc channel scan input, manually specify channels
    static auto
	ch_scan (uint32_t) -> void;

    //ADC1CHIT

    //get channel adc compare hit bit
    static auto
    ch_hit (CH0SA) -> bool;

    //get all channels adc compare hit bit
    static auto
	ch_hit () -> uint32_t;

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
