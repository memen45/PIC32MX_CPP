#pragma once

#include <cstdint>

//MCCP 1-3, SCCP 4-9

struct Ccp  {

    //instantiate Ccp with CCPn
    enum CCPX {
        CCP1, CCP2, CCP3, CCP4, CCP5, CCP6, CCP7, CCP8, CCP9
    };
    Ccp(CCPX);

//CCPxCON1

    enum OPOSTSRC : bool { TMRIRQ, EVTTRG };
    void            outscale_src    (OPOSTSRC);

    void            retrigger       (bool);
    void            outscale        (uint8_t);
    void            trig_mode       ();
    void            sync_mode       ();
    void            oneshot         (bool);
    void            sync_altout     (bool);

    enum SYNC : uint8_t {
        PR = 0, MCCPME, MCCP1, SCCP2, SCCP3, INT0 = 9, INT1, INT2, INT3, INT4,
        SCCP8, SCCP9, CLC1, CLC2, CLC3, CLC4, COMP1 = 24, COMP2, COMP3, ADC,
        TMR = 31, TRIGSOFT = 31
    };
    void            sync_src        (SYNC);

    void            on              (bool);
    void            stop_idle       (bool);
    void            run_sleep       (bool);
    void            sync_tmr        (bool);

    enum CLKSEL : uint8_t {
        SYSCLK = 0, REFO1, SOSC,
        CLC1MCCP1 = 3, CLC2MCCP2 = 3, CLC3MCCP3 = 3, CLC1SCCP4 = 3,
        CLC2SCCP5 = 3, CLC3SCCP6 = 3, CLC4SCCP7 = 3, CLC1SCCP8 = 3,
        CLC1SCCP9 = 3, TCKIB = 6, TCKIA = 7
    };
    void            clk_src         (CLKSEL);

    enum TMRPS : uint8_t { PS1, PS4, PS16, PS64 };
    void            tmr_prescale    (TMRPS);

    enum MODE : uint8_t {
        //mode          <5>     <4>        <3:0>
        //name          t32     ccsel       mod
        //timer modes
        TIMER16 =       0<<5|   0<<4|       0,
        TIMER32 =       1<<5|   0<<4|       0,
        //output compare/pwm modes
        SEHIMATCH16 =   0<<5|   0<<4|       1,
        SEHIMATCH32 =   1<<5|   0<<4|       1,
        SELOMATCH16 =   0<<5|   0<<4|       2,
        SELOMATCH32 =   1<<5|   0<<4|       2,
        SETOGMATCH16 =  0<<5|   0<<4|       3,
        SETOGMATCH32 =  1<<5|   0<<4|       3,
        DEMATCH16 =     0<<5|   0<<4|       4,
        DEPWM16 =       0<<5|   0<<4|       5,
        CENTERPWM16 =   0<<5|   0<<4|       6,
        VFPULSE16 =     0<<5|   0<<4|       7,
        //input capture modes
        CAPEDGE16 =     0<<5|   1<<4|       0,
        CAPEDGE32 =     1<<5|   1<<4|       0,
        CAPRISE16 =     0<<5|   1<<4|       1,
        CAPRISE32 =     1<<5|   1<<4|       1,
        CAPFALL16 =     0<<5|   1<<4|       2,
        CAPFALL32 =     1<<5|   1<<4|       2,
        CAPBOTH16 =     0<<5|   1<<4|       3,
        CAPBOTH32 =     1<<5|   1<<4|       3,
        CAP4RISE16 =    0<<5|   1<<4|       4,
        CAP4RISE32 =    1<<5|   1<<4|       4,
        CAP16RISE16 =   0<<5|   1<<4|       5,
        CAP16RISE32 =   1<<5|   1<<4|       5
    };
    void            mode            (MODE);

//CCPxCON2

    void            out_sync        (bool);

    enum OUTPINS : uint8_t {
        OCF = 1<<5, OCE = 1<<4, OCD = 1<<3,
        OCC = 1<<2, OCB = 1<<1, OCA = 1<<0
    };
    void            out_pins        (OUTPINS);

    enum ICGSM : uint8_t { LEVEL, RISEON, FALLOFF  };
    void            gate_mode       (ICGSM);

    enum AUXOUT : uint8_t { OFF, ROLLOVER, SIGNALOUT, CAPCOMPEVT };
    void            out_aux         (AUXOUT);

    enum ICS : uint8_t {
        ICMX, ICOMP1, ICOMP2, ICOMP3, ICLC1, ICLC2, ICLC3, ICLC4
    };
    void            cap_src         (ICS);

    void            pwm_restart     (bool);
    void            gate_auto       (bool);
    void            gate_now        (bool);

    enum GATEAUTOSRC : uint8_t {
        OCFB = 1<<7, OCFA = 1<<6,
        CLC1_MCCP1 = 1<<5, CLC2_MCCP2 = 1<<5, CLC3_MCCP3 = 1<<5,
        CLC1_SCCP4 = 1<<5, CLC2_SCCP5 = 1<<5, CLC3_SCCP6 = 1<<5,
        CLC4_SCCP7 = 1<<5, CLC1_SCCP8 = 1<<5, CLC2_SCCP9 = 1<<5,
        SCCP4_MCCP123 = 1<<4, MCCP1_SCCP4TO9 = 1<<4,
        SCCP5_MCCP123 = 1<<3, MCCP2_SCCP4TO9 = 1<<4,
        COMP_3 = 1<<2, COMP_2 = 1<<1, COMP_1 = 1<<0
    };
    void            gate_autosrc    (GATEAUTOSRC);


//CCPxCON3

    void            out_trigwait    (bool);
    void            oneshot_extend  (uint8_t);

    enum OUTM : uint8_t {
        STEERABLE = 0, PUSHPULL, HALFBRIDGE, BRUSHREV = 4, BRUSHFWD, SCAN
    };
    void            out_mode        (OUTM);

    enum POLARITY : bool { ACTHIGH, ACTLOW };
    void            polarity_ace    (POLARITY);
    void            polarity_bdf    (POLARITY);

    enum SHUTDOWN : uint8_t { HIGHIMP, INACTIVE, ACTIVE };
    void            shutdown_ace    (SHUTDOWN);
    void            shutdown_bdf    (SHUTDOWN);

    void            dead_time       (uint8_t);

//CCPxSTAT

    bool            pr16_busy       ();
    bool            tmr32_busy      ();
    bool            tmr16_busy      ();
    bool            compb_busy      ();
    bool            compa_busy      ();
    void            gate_arm        ();
    bool            stat_trig       ();
    void            trig_set        ();
    void            trig_clr        ();
    bool            stat_shutdown   ();
    bool            stat_secomp     ();
    bool            stat_capdis     ();
    bool            stat_oflow      ();
    bool            stat_bufany     ();

//CCPxTMR

    void            tmr16           (uint16_t);
    void            tmr16h          (uint16_t);
    void            tmr32           (uint32_t);
    uint16_t        tmr16           ();
    uint16_t        tmr16h          ();
    uint32_t        tmr32           ();

//CCPxPR

    void            pr16            (uint16_t);
    void            pr16h           (uint16_t);
    void            pr32            (uint32_t);
    uint16_t        pr16            ();
    uint16_t        pr16h           ();
    uint32_t        pr32            ();

//CCPxRA, CCPxRB

    void            compa           (uint16_t);
    void            compb           (uint16_t);
    void            comp32          (uint32_t);
    uint16_t        compa           ();
    uint16_t        compb           ();
    uint32_t        comp32          ();

//CCPxBUF

    uint16_t        buf16           ();
    uint32_t        buf32           ();

//misc

    uint8_t         ccp_num         ();             //which ccp number am I

    private:

    volatile uint32_t* m_ccpx_con;
};

/*
clock source -> CLKSEL -> TMRPS -> TMRSYNC -> SSDG ->

sync_mode -> tmr runs freely, resets when SYNC<> asserted, restarts
trig_mode -> tmr in reset until SYNC<> asserted, counts until TRCLR set


mode
t32     ccsel       mod     name
//timer modes
0       0           0000    TIMER16
1       0           0000    TIMER32
//output compare/pwm modes
0       0           0001    SEHIMATCH16         CCPxRA          CCPxTMR<15:0>   CCPxPR<15:0>
1       0           0001    SEHIMATCH32         CCPxRB:CCPxRA   CCPxTMR         FFFFFFFF
0       0           0010    SELOMATCH16         CCPxRA          CCPxTMR<15:0>   CCPxPR<15:0>
1       0           0010    SELOMATCH32         CCPxRB:CCPxRA   CCPxTMR         FFFFFFFF
0       0           0011    SETOGMATCH16        CCPxRA          CCPxTMR<15:0>   CCPxPR<15:0>
1       0           0011    SETOGMATCH32        CCPxRB:CCPxRA   CCPxTMR         FFFFFFFF
x       0           0100    DEMATCH16           CCPxRA (rise)   CCPxRB (fall)   CCPxTMR<15:0>   CCPxPR<15:0>
x       0           0101    DEPWM16             same as DEMATCH16, but CCPxRA/B are double buffered
x       0           0110    CENTERPWM16         CCPxRA (pwitdh) CCPxTMR<15:0>  CCPxPR<15:0>
x       0           0111    VFPULSE16           CCPxRA (adder) CCPXTMR<15:0> (accum)
//input capture modes
0       1           0000    CAPEDGE16
1       1           0000    CAPEDGE32
0       1           0001    CAPRISE16
1       1           0001    CAPRISE32
0       1           0010    CAPFALL16
1       1           0010    CAPFALL32
0       1           0011    CAPBOTH16
1       1           0011    CAPBOTH32
0       1           0100    CAP4RISE16
1       1           0100    CAP4RISE32
0       1           0101    CAP16RISE16
1       1           0101    CAP16RISE32

*/
