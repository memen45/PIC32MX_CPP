#pragma once

/*=============================================================================
 ADC
=============================================================================*/

#include <cstdint>

namespace Adc {

    enum
    {
        ADC1BUF0 = 0xBF802100,
        ADC1BUF_SPACING = 0x10,
        ADC1BUF_LAST = 21,
        ADC1CON1 = 0xBF802260,
            ON = 1<<15, SIDL = 1<<13,
            MODE12 = 1<<3, ASAM = 1<<2, SAMP = 1<<1, DONE = 1<<0,
        ADC1CON2 = 0xBF802270,
            OFFCAL = 1<<12, BUFREGEN = 1<<11, CSCNA = 1<<10, BUFS = 1<<7,
            SMPISHIFT = 2, SMPICLR = 7<<2, BUFM = 1<<0,
        ADC1CON3 = 0xBF802280,
            ADRC = 1<<15, EXTSAM = 1<<14, SAMCSHIFT = 8, SAMCCLR = 31<<8,
        ADC1CHS = 0xBF802290,
        ADC1CSS = 0xBF8022A0,
        ADC1CON5 = 0xBF8022C0,
            ASEN = 1<<15, LPEN = 1<<14, BGREQ = 1<<12, WMCLR = 3<<2,
        ADC1CHIT = 0xBF8022D0,


    };

    //enum argument types

    enum FORM //AD1CON1
    {
        INT16 = 0<<8, SINT16 = 1<<8, FR16 = 2<<8, SFR16 = 3<<8,
        INT32 = 4<<8, SINT32 = 5<<8, FR32 = 6<<8, SFR32 = 7<<8
    };

    enum SSRC //AD1CON1
    {
        SAMPCLR = 0<<4,
        SOFT = 0<<4,
        INT0 = 1<<4,
        TMR3 = 2<<4, TMR1 = 5<<4, TMR1SLEP = 6<<4,
        AUTO = 7<<4,
        MCCP1 = 8<<4, MCCP2 = 9<<4, MCCP3 = 10<<4,
        SCCP4 = 11<<4, SCCP5 = 12<<4, SCCP6 = 13<<4,
        CLC1 = 14<<4, CLC2 = 15<<4
    };

    enum VCFG
    {
        CFGCLR = 7<<13,
        EXTP_EXTN = 3<<13, EXTP_VSS = 2<<13, VDD_EXTN = 1<<13, VDD_VSS = 0<<13
    };

    enum ASINT
    {
        NONE = 0<<8, DET = 1<<8, COMP = 2<<8, DETCOMP = 3<<8

    };

    enum WM
    {
        LEGACY = 0<<2, CONVSAV = 1<<2, COMPONLY = 2<<2
    };

    enum CM
    {
        LT = 0, GT = 1, INWIN = 2, OUTWIN = 3
    };

    enum CH0SA
    {
        AN0 = 0, AN1, AN2, AN3, AN4, AN5, AN6, AN7, AN8, AN9, AN10, AN11, AN12,
        AN13, AN14, AN15, AN16, AN17, AN18, AN19,
        VDD = 27, VBG, AVSS, AVDD, END = 255
    };

    //functions

    //ADC1BUFn
    uint16_t bufn( uint8_t n )
    {
        if( n > ADC1BUF_LAST ) n = ADC1BUF_LAST;
        return Reg::val16( ADC1BUF0+(n*ADC1BUF_SPACING) );

    }

    //ADC1CON1
    void on( bool tf ){                 Reg::set( ADC1CON1, ON, tf ); }
    void stop_idle( bool tf ){          Reg::set( ADC1CON1, SIDL, tf ); }
    void format( FORM e )
    {
        Reg::clr( ADC1CON1, SFR32 );
        Reg::set( ADC1CON1, e );
    }
    void trig_sel( SSRC e )
    {
        Reg::clr( ADC1CON1, CLC2 );
        Reg::set( ADC1CON1, e );
    }
    void mode_12bit( bool tf ){         Reg::set( ADC1CON1, MODE12, tf ); }
    void samp_auto( bool tf ){          Reg::set( ADC1CON1, ASAM, tf ); }
    void samp( bool tf ){               Reg::set( ADC1CON1, SAMP, tf ); }
    bool samp( void ){           return Reg::is_set( ADC1CON1, SAMP ); }
    bool done( void ){           return Reg::is_set( ADC1CON1, DONE ); }

    //ADC1CON2
    void vref_cfg( VCFG e )
    {
        Reg::clr( ADC1CON2, CFGCLR );
        Reg::set( ADC1CON2, e );
    }
    void offset_cal( bool tf ){         Reg::set( ADC1CON2, OFFCAL, tf ); }
    void buf_reg( bool tf ){            Reg::set( ADC1CON2, BUFREGEN, tf ); }
    void scan( bool tf ){               Reg::set( ADC1CON2, CSCNA, tf ); }
    bool buf2nd_busy( void ){          Reg::is_set( ADC1CON2, BUFS ); }
    void nper_irq( uint8_t n )
    {
        n -= 1; n &= 15; //n = 1-16
        Reg::clr( ADC1CON2, SMPICLR );
        Reg::set( ADC1CON2, n<<SMPISHIFT );
    }
    void buf_split( bool tf ){          Reg::set( ADC1CON2, BUFM, tf ); }

    //ADC1CON3
    void clk_frc( bool tf ){            Reg::set( ADC1CON3, ADRC, tf ); }
    void samp_extend( bool tf ){        Reg::set( ADC1CON3, EXTSAM, tf ); }
    void samp_time( uint8_t t )
    {
        if( t == 0 ) t = 1; //0 not allowed
        t &= 31;
        Reg::clr( ADC1CON3, SAMCCLR );
        Reg::set( ADC1CON3, t<<SAMCSHIFT );
    }
    //default value is for 24MHz, 4 will meet 280ns Tad for any clock
    void conv_time( uint8_t t = 4 )
    {
        Reg::clr( ADC1CON3, 255 );
        Reg::set( ADC1CON3, t );
    }

    //ADC1CON5
    void scan_auto( bool tf ){          Reg::set( ADC1CON5, ASEN, tf ); }
    void lp_afterscan( bool tf ){       Reg::set( ADC1CON5, LPEN, tf ); }
    void bandgap( bool tf ){            Reg::set( ADC1CON5, BGREQ, tf ); }
    void scan_autoirq( ASINT e )
    {
        Reg::clr( ADC1CON5, DETCOMP );
        Reg::set( ADC1CON5, e );
    }
    void write_mode( WM e )
    {
        Reg::clr( ADC1CON5, WMCLR );
        Reg::set( ADC1CON5, e );
    }
    void compare_mode( CM e )
    {
        Reg::clr( ADC1CON5, OUTWIN );
        Reg::set( ADC1CON5, e );
    }

    //ADC1CHS
    void ch_sel( CH0SA e ){                     Reg::val16( ADC1CHS, e );}

    //ADC1SS
    void ch_scan( CH0SA e, bool tf ){           Reg::set( ADC1CSS, e, tf ); }
    void ch_scan( CH0SA* e )
    {
        Reg::val16( ADC1CSS, 0 ); //clr all
        for( ; *e != END; e++ ){
            Reg::set( ADC1CSS, 1<<*e ); //set list
        }
    }
    void ch_scan( uint32_t v ){                 Reg::val( ADC1CSS, v ); }

    //ADC1CHIT
    //non AN values will return 0 (like VDD)
    bool ch_hit( CH0SA e ){         return Reg::is_set( ADC1CHIT, 1<<e ); }
    uint32_t ch_hit( void ){        return Reg::val( ADC1CHIT ); }
}





/*
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