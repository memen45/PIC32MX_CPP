#pragma once

//I/O pins

#include <cstdint>
#include "Reg.hpp"
#include "Sys.hpp"
#include "Delay.hpp"

class Pins : public Delay {

    public:

    //pps_in peripheral names
    enum PPSIN : uint8_t {
        //byte offset from RPINR1
        INT4 = 0,                                   //R1
        ICM1 = 6, ICM2 = 7,                         //R2
        ICM3 = 8, ICM4 = 9,                         //R3

        OCFA = 14, OCFB = 15,                       //R5
        TCKIA = 16, TCKIB = 17,                     //R6
        ICM5 = 20, ICM6 = 21, ICM7 = 22, ICM8 = 23, //R7
        ICM9 = 24, U3RX = 27,                       //R8
        U2RX = 30, U2CTS = 31,                      //R9
        U3CTS = 35,                                 //R10
        SDI2 = 36, SCK2IN = 37, SS2IN = 38,         //R11
        CLCINA = 42, CLCINB = 43,                   //R12
        PPSINOFF = 255
    };

    //pps_out peripheral names
    enum PPSOUT : uint8_t {
        PPSLAT = 0,
        C1OUT, C2OUT, C3OUT,
        U2TX, U2RTS, U3TX, U3RTS,
        SDO2, SCK2OUT, SS2OUT,
        OCM4, OCM5, OCM6, OCM7, OCM8, OCM9,
        CLC1OUT, CLC2OUT, CLC3OUT, CLC4OUT,
    };

    enum : uint8_t {
        PTSHIFT = 4, PTMASK = 3,
        PNSHIFT = 0, PNMASK = 15,
        RPSHIFT = 6, RPMASK = 31,
        ANSHIFT = 11, ANMASK = 31
    };

    //A0 type name, or RP0 type name (equivilent)
    enum RPN : uint16_t {
        // encode as- 0xaaaaarrrrrppnnnn - | ANn | RPn | PORT | PIN |
        // aaaaa = ANn = 0-19
        // pp = port A=0,B=1,C=2,D=3
        // nnnn = pin = 0-15
        // rrrrr = RPn = 1-24

        //pins without AN will be set to use AVss (29) as AN input
        //(since AN0 is 0, we need something other than 0 to signify no AN for pin, so
        // we will just give it something that will result in no result if ever used
        // by mistake)

        //A0 - A-15
        A0 = 0|1<<RPSHIFT|0<<ANSHIFT, A1 = 1|2<<RPSHIFT|1<<ANSHIFT,
        A2 = 2|3<<RPSHIFT|5<<ANSHIFT, A3 = 3|4<<RPSHIFT|6<<ANSHIFT,
        A4 = 4|5<<RPSHIFT|29<<ANSHIFT, A5 = 5|29<<ANSHIFT,
        A6 = 6|19<<ANSHIFT, A7 = 7|21<<RPSHIFT|29<<ANSHIFT,
        A8 = 8|29<<ANSHIFT, A9 = 9|24<<RPSHIFT|29<<ANSHIFT,
        A10 = 10|22<<RPSHIFT|29<<ANSHIFT, A11 = 11|18<<ANSHIFT,
        A12 = 12|17<<ANSHIFT, A13 = 13|16<<ANSHIFT,
        A14 = 14|29<<ANSHIFT, A15 = 15|29<<ANSHIFT,
        //B0-B15
        B0 = 16|6<<RPSHIFT|2<<ANSHIFT, B1 = 17|7<<RPSHIFT|3<<ANSHIFT,
        B2 = 18|8<<RPSHIFT|4<<ANSHIFT, B3 = 19|9<<RPSHIFT|11<<ANSHIFT,
        B4 = 20|10<<RPSHIFT|7<<ANSHIFT, B5 = 21|11<<RPSHIFT|29<<ANSHIFT,
        B6 = 22|29<<ANSHIFT, B7 = 23|12<<RPSHIFT|29<<ANSHIFT,
        B8 = 24|13<<RPSHIFT|29<<ANSHIFT, B9 = 25|14<<RPSHIFT|29<<ANSHIFT,
        B10 = 26|29<<ANSHIFT, B11 = 27|29<<ANSHIFT,
        /*no B12*/ B13 = 29|15<<RPSHIFT|8<<ANSHIFT,
        B14 = 30|16<<RPSHIFT|9<<ANSHIFT, B15 = 31|17<<RPSHIFT|10<<ANSHIFT,
        //C0-C15
        C0 = 32|12<<ANSHIFT, C1 = 33|13<<ANSHIFT,
        C2 = 34|19<<RPSHIFT|29<<ANSHIFT, C3 = 35|29<<ANSHIFT,
        C4 = 36|29<<ANSHIFT, C5 = 37|15<<ANSHIFT,
        C6 = 38|23<<RPSHIFT|29<<ANSHIFT, C7 = 39|20<<RPSHIFT|29<<ANSHIFT,
        C8 = 40|18<<RPSHIFT|14<<ANSHIFT, C9 = 41|29<<ANSHIFT,
        C10 = 42|29<<ANSHIFT, C11 = 43|29<<ANSHIFT,
        C12 = 44|29<<ANSHIFT, C13 = 45|29<<ANSHIFT,
        C14 = 46|29<<ANSHIFT, C15 = 47|29<<ANSHIFT,
        //D0-D4
        D0 = 48|29<<ANSHIFT, D1 = 49|29<<ANSHIFT,
        D2 = 50|29<<ANSHIFT, D3 = 51|29<<ANSHIFT,
        D4 = 52|29<<ANSHIFT,

        //RPn 1-24
        RP1 = A0, RP2 = A1, RP3 = A2, RP4 = A3,
        RP5 = A4, RP6 = B0, RP7 = B1, RP8 = B2,
        RP9 = B3, RP10 = B4, RP11 = B5, RP12 = B7,
        RP13 = B8, RP14 = B9, RP15 = B13, RP16 = B14,
        RP17 = B15, RP18 = C8, RP19 = C2, RP20 = C7,
        RP21 = A7, RP22 = A10, RP23 = C6, RP24 = A9,

        //ANn 0-19
        AN0 = A0, AN1 = A1, AN2 = B0, AN3 = B1, AN4 = B2, AN5 = A2, AN6 = A3,
        AN7 = B4, AN8 = B13, AN9 = B14, AN10 = B15, AN11 = B3, AN12 = C0,
        AN13 = C1, AN14 = C8, AN15 = C5, AN16 = A13, AN17 = A12, AN18 = A11,
        AN19 = A6
    };

    //r/w pins
    bool        pinval          () const;
    bool        latval          () const;
    void        latval          (bool) const;
    void        low             () const;
    void        high            () const;
    void        invert          () const;
    void        on              () const;
    void        off             () const;
    bool        ison            () const;

    //pin modes
    void        lowison         (bool);
    void        digital_in      () const;
    void        analog_in       () const;
    void        digital_out     () const;
    void        odrain          (bool) const;
    void        pullup          (bool) const;
    void        pulldn          (bool) const;

    //icn
    void        icn             (bool) const;
    void        icn_rising      () const;
    void        icn_falling     () const;
    void        icn_risefall    () const;
    void        icn_mismatch    () const;
    bool        icn_flag        () const;
    bool        icn_stat        () const;
    void        icn_flagclr     () const;

    //pps
    void        pps_in          (PPSIN);
    void        pps_out         (PPSOUT);

    //get adc #
    uint8_t     an_num          ();

    // bit  | 4  | 3  |  2   |  1   |  0  |
    //      | PD | PU | ACTL | DOUT | DIN |
    //-----------------------------------------
    //AIN   |    |    |      |      |     | 0
    //IN    |    |    |      |      |  1  | 1
    //INPU  |    |  1 |  1   |      |  1  | 13
    //INPD  |  1 |    |      |      |  1  | 17
    //INL   |    |    |  1   |      |  1  | 5
    //OUT   |    |    |      |   1  |     | 2
    //OUTL  |    |    |  1   |   1  |     | 6
    enum IOMODE : uint8_t {
        AIN = 0,
        IN = 1, INPU = 1<<3|1<<2|IN, INPD = 1<<4|IN, INL = 1<<2|IN,
        OUT = 2, OUTL = 1<<2|OUT
    };
    enum : uint8_t { ACTL = 1<<2  };

    //constructor
                Pins            (RPN, IOMODE = AIN);

    private:

    static Reg r;
    static Sys sys;

    enum { //offsets from base address, in words
        TRIS = 0x10>>2, PORT = 0x20>>2, LAT = 0x30>>2, ODC = 0x40>>2,
        CNPU = 0x50>>2, CNPD = 0x60>>2, CNCON = 0x70>>2, CNEN0 = 0x80>>2,
        CNSTAT = 0x90>>2, CNEN1 = 0xA0>>2, CNF = 0xB0>>2
    };

    enum {
        ANSELA = 0xBF802BB0, ANSELX_SPACING = 64, //spacing in words
        //CNCONx
            ON = 1<<15,
            CNSTYLE = 1<<11,
        RPCON = 0xBF802A00,
            IOLOCK = 1<<11,
        RPINR1 = 0xBF802A10,
        RPOR0 = 0xBF802B10
    };

    static void     pps_do      (uint32_t, uint8_t);

    volatile uint32_t*  m_pt;       //base address
    const uint16_t      m_pn;       //pin mask
    bool                m_lowison;  //pin on val is low
    const uint8_t       m_rpn;      //RPn value
    uint8_t             m_ppsin;    //store ppsin peripheral
    const uint8_t       m_an;       //ANn value

    using vu32_ptr = volatile uint32_t*;

};
