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
        INT16 = 0, SINT16, FR16, SFR16, INT32, SINT32, FR32, SFR32
    };
    static void         format          (FORM);

    //adc conversion trigger source select
    enum SSRC : uint8_t {
        SOFT = 0, INT0, TMR3, CTMU, AUTO = 7,
    };
    static void         trig_sel        (SSRC);

    //true = stop conversions on interrupt, 
	//false = buffer contents overwritten by next conversion
    static void         clr_asam      (bool);

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
		VDD_VSS = 0, EXTP_VSS, VDD_EXTN, EXTP_EXTN
    };
    static void         vref_cfg        (VCFG);

    //connect SHA inputs to negative reference for offset calibration
    static void         offset_cal      (bool);

    //enable channel scan
    static void         scan            (bool);

    //true = second half of buffers busy, false = first half busy
    static bool         buf2nd_busy     ();

    //number of samples per irq, 1-16
    static void         samp_nirq       (uint8_t);

    //split buffer into two halves
    static void         buf_split       (bool);
	
	//Alternate input for subsequent samples
	static void			alt_input		(bool);

//ADC1CON3

    //set adc conversion clock source
    enum CLK { PBCLK = 0, FRC };
    static void         clk_src         (CLK);

    //auto sample time, 1-31 Tad
    static void         samp_time       (uint8_t);

    //set conversion clock time Tad, default = 7 (max needed)
    static void         conv_time       (uint8_t = 7);

//ADC1CHS

    //adc input channel select
    enum CH0S : uint8_t {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7,
        AN8, AN9, AN10, AN11, AN12,AN13, AN14, AN15,
		END = 255
    };
	enum CH0N : uint8_t { VREFL = 0 /*, AN1 already defined */ };
    static void         ch_selA         (CH0S, CH0N = VREFL);
    static void         ch_selB         (CH0S, CH0N = VREFL);

//AD1CSSL

    //enable adc channel scan input, one channel
    static void         ch_scan         (CH0S, bool);

    //enable adc channel scan input, one or more channels (in array)
    static void         ch_scan         (CH0S*);

    //enable adc channel scan input, manually specify channels
    static void         ch_scan         (uint32_t);

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
