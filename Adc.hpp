#pragma once

//ADC

#include <cstdint>

struct Adc {

            //==== ADC1BUFn ====

            //read adc buffer N, N = 0 - ADC1BUF_LAST (21), N default = 0
            //returning register contents, caller has to determine format
            static auto
read        (uint8_t = 0) -> uint32_t;


            //==== ADC1CON1 ====

            //enable adc
            static auto
on          (bool) -> void;

            //enable stop adc when in idle mode
            static auto
stop_idle   (bool) -> void;

            enum
FORM        : uint8_t {
            INT16, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
            };

            //adc data output format
            static auto
format      (FORM) -> void;

            enum
SSRC        : uint8_t {
            SOFT = 0, INT0, TMR3, CTMU, AUTO = 7,
            };

            //adc conversion trigger source select
            static auto
trig_sel    (SSRC) -> void;

            //true = 12bit mode, false = 10bit mode
            static auto
clr_asam    (bool) -> void;

            //true = auto start, false = wait for samp bit set
            static auto
samp_auto   (bool) -> void;

            //true = adc sample enable, false = end sample, start conversion
            static auto
samp        (bool) -> void;

            //get adc sample status
            static auto
samp        () -> bool;

            //get adc conversion status, true = done
            static auto
done        () -> bool;


            //==== ADC1CON2 ====
            enum
VCFG        : uint8_t {
            VDD_VSS, EXTP_VSS, VDD_EXTN, EXTP_EXTN
            };

            //set Vr+/Vr- adc voltage reference
            static auto
vref_cfg    (VCFG) -> void;
	
            //connect SHA inputs to negative reference
            static auto
offset_cal  (bool) -> void;

            //enable channel scan
            static auto
scan        (bool) -> void;

            //true = second half busy, false = first half busy
            static auto
buf2nd_busy () -> bool;

            //number of samples per irq, 1-16
            static auto
samp_nirq   (uint8_t) -> void;

            //split buffer into two halves
            static auto
buf_split   (bool) -> void;

            //Alternate input for subsequent samples
            static auto
alt_input   (bool) -> void;

            //==== ADC1CON3 ====

            enum
CLK         { PBCLK, FRC };

            //set adc conversion clock source
            static auto
clk_src     (CLK) -> void;

            //auto sample time, 1-31 Tad
            static auto
samp_time   (uint8_t) -> void;

            //set conversion clock time Tad, default = 7 (max needed)
            static auto
conv_time   (uint8_t = 7) -> void;

            //==== ADC1CHS ====

            enum
CH0S        : uint32_t {
            AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7,
            AN8, AN9, AN10, AN11, AN12, AN13, AN14, AN15,
            END = 255
            };
            
            enum
CH0N        : uint32_t {
                VREFL /*, AN1, */
            };
            
            enum
SAMPLE      : bool {
                A, B
            };

            //adc input channel select
            static auto
ch_sel      (CH0S, CH0N = VREFL, SAMPLE = A) -> void;


            //==== ADC1SS ====
            
            //enable adc channel scan input, 1 channel
            static auto
ch_scan     (CH0S, bool) -> void;

            //enable adc channel scan input, (1 or more, via array of CH0SA)
            static auto
ch_scan     (CH0S*) -> void;

            //enable adc channel scan input, (set reg bits for each channel)
            static auto
ch_scan     (uint32_t) -> void;

};


/*
 misc info

10bit mode
Tad     ADC Clock Period    200ns
Tconv   Conversion Time     12Tad
Tpss    Sample Start Delay  1Tad
Fcnv    Throughput          400ksps 

10bit mode with external Vref+/- 
Tad		ADC Clock Period	65ns
Tsamp	Sampling time		132ns
Fcnv	Throughput			1Msps to 400ksps

Tsrc    FastRC 8Mhz         1/8MHz  = 125ns (behavior with this clock source is not documented
Tpb    PBclk 40MHz         1/40MHz = 25ns

Tad =   (adcs<7:0> + 1) * 2 * Tpb
        (adcs=0, then Tad = 2 * Tpb)

Tpb = 8MHz (Frc)
adcs = 0 -> 1 * 2 * 125ns = 250ns <-min use for 10bit 
adcs = 1 -> 2 * 2 * 125ns = 500ns
adcs = 2 -> 3 * 2 * 125ns = 750ns
adcs = 3 -> 4 * 2 * 125ns = 1000ns
adcs = 4 -> 5 * 2 * 125ns = 1250ns
adcs = 5 -> 6 * 2 * 125ns = 1500ns
adcs = 6 -> 7 * 2 * 125ns = 1750ns
adcs = 255 -> 256 * 2 * 125ns = 64us

Tpb = 40MHz (PBclk)
adcs = 0 -> 1 * 2 * 25ns = 50ns
adcs = 1 -> 2 * 2 * 25ns = 100ns <-min use for 10bit 40MHz with external Vref
adcs = 2 -> 3 * 2 * 25ns = 150ns
adcs = 3 -> 4 * 2 * 25ns = 200ns <-min use for 10bit 40MHz
adcs = 4 -> 5 * 2 * 25ns = 250ns
adcs = 5 -> 6 * 2 * 25ns = 300ns
adcs = 6 -> 7 * 2 * 25ns = 350ns
adcs = 255 -> 256 * 2 * 25ns = 12.8us

Tpb = 80MHz (PBclk max)
adcs = 0 -> 1 * 2 * 12.5ns = 25ns
adcs = 1 -> 2 * 2 * 12.5ns = 50ns
adcs = 2 -> 3 * 2 * 12.5ns = 75ns <-min use for 10bit 80MHz with external Vref
adcs = 3 -> 4 * 2 * 12.5ns = 100ns
adcs = 4 -> 5 * 2 * 12.5ns = 125ns
adcs = 5 -> 6 * 2 * 12.5ns = 150ns
adcs = 6 -> 7 * 2 * 12.5ns = 175ns
adcs = 7 -> 8 * 2 * 12.5ns = 200ns <-min use for 10bit 80MHz
adcs = 255 -> 256 * 2 * 12.5ns = 6.4us

*/
