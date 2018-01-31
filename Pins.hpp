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

    //A0 type name, or RP0 type name (equivilent)
    enum RPN : uint16_t {
        // encode as- 0x000rrrrr00ppnnnn - | 000 RPn | 00 PORT/PIN |
        // pp = port A=0,B=1,C=2,D=3
        // nnnnn = pin = 0-15
        // rrrrr = RPn = 1-24

        //A0 - A-15  0x000rrrrr00ppnnnn
        A0 = 0|1<<8, A1 = 1|2<<8, A2 = 2|3<<8, A3 = 3|4<<8,
        A4 = 4|5<<8, A5 = 5, A6 = 6, A7 = 7|21<<8,
        A8 = 8, A9 = 9|24<<8, A10 = 10|22<<8, A11 = 11,
        A12 = 12, A13 = 13, A14 = 14, A15 = 15,
        //B0-B15
        B0 = 16|6<<8, B1 = 17|7<<8, B2 = 18|8<<8, B3 = 19|9<<8,
        B4 = 20|10<<8, B5 = 21|11<<8, B6 = 22, B7 = 23|12<<8,
        B8 = 24|13<<8, B9 = 25|14<<8, B10 = 26, B11 = 27,
        /*no B12*/ B13 = 29|15<<8, B14 = 30|16<<8, B15 = 31|17<<8,
        //C0-C15
        C0 = 32, C1 = 33, C2 = 34|19<<8, C3 = 35,
        C4 = 36, C5 = 37, C6 = 38|23<<8, C7 = 39|20<<8,
        C8 = 40|18<<8, C9 = 41, C10 = 42, C11 = 43,
        C12 = 44, C13 = 45, C14 = 46, C15 = 47,
        //D0-D4
        D0 = 48, D1 = 49, D2 = 50, D3 = 51,
        D4 = 52,

        //RPn 1-24
        RP1 = A0, RP2 = A1, RP3 = A2, RP4 = A3,
        RP5 = A4, RP6 = B0, RP7 = B1, RP8 = B2,
        RP9 = B3, RP10 = B4, RP11 = B5, RP12 = B7,
        RP13 = B8, RP14 = B9, RP15 = B13, RP16 = B14,
        RP17 = B15, RP18 = C8, RP19 = C2, RP20 = C7,
        RP21 = A7, RP22 = A10, RP23 = C6, RP24 = A9
    };

    //r/w pins
    bool        pinval          () const;
    bool        latval          () const;
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

    // bit  | 4  | 3  |  2   |  1   |  0  |
    //      | PD | PU | ACTL | DOUT | DIN |
    //-----------------------------------------
    //AIN   |    |    |      |      |     | 0
    //DIN   |    |    |      |      |  1  | 1
    //DINPU |    |  1 |  1   |      |  1  | 13
    //DINPD |  1 |    |      |      |  1  | 17
    //DINL  |    |    |  1   |      |  1  | 5
    //DOUT  |    |    |      |   1  |     | 2
    //DOUTL |    |    |  1   |   1  |     | 6
    enum IOMODE : uint8_t {
        AIN = 0,
        DIN = 1, DINPU = 1<<3|1<<2|DIN, DINPD = 1<<4|DIN, DINL = 1<<2|DIN,
        DOUT = 2, DOUTL = 1<<2|DOUT
    };

    //constructor
                Pins            (RPN, IOMODE = AIN);

    private:

    static Reg r;
    static Sys sys;

    enum { //offsets from base address, in words
        TRIS = 0x10>>2, PORT_ = 0x20>>2, LAT = 0x30>>2, ODC = 0x40>>2,
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
        RPOR0 = 0xBF802B10,
        //misc
        PTMASK = 3, PTSHIFT = 4, PNMASK = 15,
        ACTL = 1<<2 //IOMODE
    };

    static void     pps_do      (uint32_t, uint8_t);

    volatile uint32_t*  m_pt;       //base address
    const uint16_t      m_pn;       //pin mask
    bool                m_lowison;  //pin on val is low
    const uint8_t       m_rpn;      //RPn value
    uint8_t             m_ppsin;    //store ppsin peripheral

    using vu32_ptr = volatile uint32_t*;

};


// A0/RP1 format - Pins led1(A0); or Pins led2(RP1, DOUT);
// m = AIN,DIN,DINPU,DINPD,DINL,DOUT,DOUTL (default is AIN)
// RPN enum encoded as 0x000rrrrr00ppnnnn (r=RPn, pp=PORT, nnnn=PIN)
//=============================================================================
                    Pins::Pins          (RPN e, IOMODE m)
//=============================================================================
    : m_pt((vu32_ptr)ANSELA + ((e>>PTSHIFT) bitand PTMASK)*ANSELX_SPACING),
      m_pn(1<<(e bitand PNMASK)),
      m_lowison(m & ACTL),
      m_rpn((uint8_t)(e>>8)),
      m_ppsin(PPSINOFF)
{
    if(m == AIN) analog_in();
    else if(m bitand DIN) digital_in();
    else digital_out();
    pullup(m == DINPU);
    pulldn(m == DINPD);
}

//=============================================================================
    bool        Pins::pinval        () const
//=============================================================================
{
    return r.anybit(m_pt+PORT_, m_pn);
}

//=============================================================================
    bool        Pins::latval        () const
//=============================================================================
{
    return r.anybit(m_pt+LAT, m_pn);
}

//=============================================================================
    void        Pins::low           () const
//=============================================================================
{
    r.clrbit(m_pt+LAT, m_pn);
}

//=============================================================================
    void        Pins::high          () const
//=============================================================================
{
    r.setbit(m_pt+LAT, m_pn);
}

//=============================================================================
    void        Pins::invert        () const
//=============================================================================
{
    r.flipbit(m_pt+LAT, m_pn);
}

//=============================================================================
    void        Pins::on            () const
//=============================================================================
{
    r.setbit(m_pt+LAT, m_pn, not m_lowison);
}

//=============================================================================
    void        Pins::off           () const
//=============================================================================
{
    r.setbit(m_pt+LAT, m_pn, m_lowison);
}

//=============================================================================
    bool        Pins::ison          () const
//=============================================================================
{
    return m_lowison ? not pinval() : pinval();
}

//=============================================================================
    void        Pins::icn_flagclr () const
//=============================================================================
{
    r.clrbit(m_pt+CNF, m_pn);
}