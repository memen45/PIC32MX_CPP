#pragma once

//ADC

#include <cstdint>

struct Adc {

    //ADC1BUFn

        static auto
    read (uint8_t = 0) -> uint16_t; //read adc buffer N,
                                    //N = 0 - ADC1BUF_LAST (21) , N default = 0
    //ADC1CON1

        static auto
    on (bool) -> void;              //enable adc

        static auto
    stop_idle (bool) -> void;       //enable stop adc when in idle mode

        enum
    FORM : uint8_t {
        INT16, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
        };

        static auto
    format (FORM) -> void;          //adc data output format

        enum
    SSRC : uint8_t {
        SOFT = 0, INT0, TMR3, TMR1 = 5, TMR1SLEP, AUTO,
        MCCP1, MCCP2, MCCP3, SCCP4, SCCP5, SCCP6, CLC1, CLC2
        };

        static auto
    trig_sel (SSRC) -> void;        //adc conversion trigger source select

        static auto
    mode_12bit (bool) -> void;      //true = 12bit, false = 10bit

        static auto
	samp_auto (bool) -> void;       //true=auto start, false=wait for samp bit set

        static auto
    samp (bool) -> void;            //true = adc sample enable,
                                    //false = end sample, start conversion
        static auto
    samp () -> bool;                //get adc sample status

        static auto
    done () -> bool;                //get adc conversion status, true = done

    //ADC1CON2

        enum
    VCFG : uint8_t {
        VDD_VSS, VDD_EXTN, EXTP_VSS, EXTP_EXTN
        };

        static auto
	vref_cfg (VCFG) -> void;        //set Vr+/Vr- adc voltage reference

        static auto
    offset_cal (bool) -> void;      //connect SHA inputs to negative reference

        static auto
	buf_reg (bool) -> void;         //true=N channel adc-> buffer N, false=FIFO

        static auto
	scan (bool) -> void;            //enable channel scan

        static auto
    buf2nd_busy () -> bool;         //true=second half busy, false=first half

        static auto
	samp_nirq (uint8_t) -> void;    //number of samples per irq, 1-16

        static auto
	buf_split (bool) -> void;       //split buffer into two halves

    //ADC1CON3

        enum
    CLK { PBCLK, FRC };

        static auto
    clk_src (CLK) -> void;          //set adc conversion clock source

        static auto
    samp_extend (bool) -> void;     //keep sampling even when samp=0

        static auto
    samp_time (uint8_t) -> void;    //auto sample time, 1-31 Tad

        static auto
	conv_time (uint8_t = 4) -> void;//set conversion clock time Tad,
                                    //default = 4 (max needed)
    //ADC1CON5

        static auto
    scan_auto (bool) -> void;       //auto scan enable

        static auto
    low_power (bool) -> void;       //enable low power after scan

        static auto
	bandgap (bool) -> void;         //enable bandgap when adc on

        enum
    ASINT : uint8_t { NONE, DET, COMP, DETCOMP };

        static auto
	scan_autoirq (ASINT) -> void;   //auto scan interrupt modes

        enum
    WM : uint8_t { LEGACY, CONVSAV, COMPONLY };

        static auto
	write_mode (WM) -> void;        //auto scan write mode

        enum
    CM :uint8_t { LT, GT, INWIN, OUTWIN };

        static auto
	compare_mode (CM) -> void;      //auto scan compare mode

    //ADC1CHS

        enum
    CH0SA : uint8_t {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7,
        AN8, AN9, AN10, AN11, AN12,AN13, AN14, AN15,
        AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD, END = 255
        };

        static auto
	ch_sel (CH0SA) -> void;         //adc input channel select

    //ADC1SS

        static auto
    ch_scan (CH0SA, bool) -> void;  //enable adc channel scan input, 1 channel

        static auto
    ch_scan (CH0SA*) -> void;       //enable adc channel scan input, (1+, array)

        static auto
	ch_scan (uint32_t) -> void;     //enable adc channel scan input, (set ch bits)

    //ADC1CHIT

        static auto
    ch_hit (CH0SA) -> bool;         //get channel adc compare hit bit

        static auto
	ch_hit () -> uint32_t;          //get all channels adc compare hit bit

    //minimal conversion times for frc/pbclk(24MHz) for 10/12bits
        enum
    CONVTIME { FRC10BIT = 1, FRC12BIT, PBCLK10BIT, PBCLK12BIT };
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
