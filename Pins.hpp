#pragma once

//I/O pins

#include <cstdint>
#include "Reg.hpp"
#include "Sys.hpp"

class Pins {

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

    //normal pin names, RXn name, or RPn name
    enum RPN : uint16_t {
        // encode as- 0x000ppnnnn000rrrrr - | 000 PORT/PIN | 000 RPn |
        // pp = port A=0,B=1,C=2,D=3
        // nnnnn = pin = 0-15
        // rrrrr = RPn = 1-24
        // so we can use one enum to get RPn and port/pin info

        //A0 - D4  (ppnnnn in 0x000ppnnnn000rrrrr)
        A0 = 0<<8, A1 = 1<<8, A2 = 2<<8, A3 = 3<<8, A4 = 4<<8, A5 = 5<<8,
        A6 = 6<<8, A7 = 7<<8, A8 = 8<<8, A9 = 9<<8, A10 = 10<<8, A11 = 11<<8,
        A12 = 12<<8, A13 = 13<<8, A14 = 14<<8, A15 = 15<<8,
        B0 = 16<<8, B1 = 17<<8, B2 = 18<<8, B3 = 19<<8, B4 =20<<8, B5 = 21<<8,
        B6 = 22<<8, B7 = 23<<8, B8 = 24<<8, B9 = 25<<8, B10 = 26<<8,
        B11 = 27<<8, /*no B12*/ B13 = 29<<8, B14 = 30<<8, B15 = 31<<8,
        C0 = 32<<8, C1 = 33<<8, C2 = 34<<8, C3 = 35<<8, C4 = 36<<8, C5 = 37<<8,
        C6 = 38<<8, C7 = 39<<8, C8 = 40<<8, C9 = 41<<8, C10 = 42<<8,
        C11 = 43<<8, C12 = 44<<8, C13 = 45<<8, C14 = 46<<8, C15 = 47<<8,
        D0 = 48<<8, D1 = 49<<8, D2 = 50<<8, D3 = 51<<8, D4 = 52<<8,

        //RPn 1-24 (rrrrr in 0x000ppnnnn000rrrrr)
        RP1 = 1+A0, RP2 = 2+A1, RP3 = 3+A2, RP4 = 4+A3,
        RP5 = 5+A4, RP6 = 6+B0, RP7 = 7+B1, RP8 = 8+B2,
        RP9 = 9+B3, RP10 = 10+B4, RP11 = 11+B5,
        RP12 = 12+B7, RP13 = 13+B8, RP14 = 14+B9,
        RP15 = 15+B13, RP16 = 16+B14, RP17 = 17+B15,
        RP18 = 18+C8, RP19 = 19+C2, RP20 = 20+C7,
        RP21 = 21+A7, RP22 = 22+A10, RP23 = 23+C6,
        RP24 = 24+A9,

        //RXn to RPn (rrrrr in 0x000ppnnnn000rrrrr)
        RA0 = RP1, RA1 = RP2, RA2 = RP3, RA3 = RP4, RA4 = RP5, RB0 = RP6,
        RB1 = RP7, RB2 = RP8, RB3 = RP9, RB4 = RP10, RB5 = RP11, RB7 = RP12,
        RB8 = RP13, RB9 = RP14, RB13 = RP15, RB14 = RP16, RB15 = RP17,
        RC8 = RP18, RC2 = RP19, RC7 = RP20, RA7 = RP21, RA10 = RP22,
        RC6 = RP23, RA9 = RP24
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

    //constructors (bool-> lowison)
    constexpr   Pins            (RPN, bool = false);    //A0, RP8, RB2, etc.

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
        RPN_MASK = 7, RN_SHIFT = 8, PINMAX = 16
    };

    static void     pps_do      (uint32_t, uint8_t);

    volatile uint32_t*  m_pt;       //base address
    const uint16_t      m_pn;       //pin mask
    bool                m_lowison;  //pin on val is low
    const uint8_t       m_rpn;      //RPn value
    uint8_t             m_ppsin;    //store ppsin peripheral

};


// A0/RA0/RP1 format - Pins led1(RA0); or Pins led2(RP1, true);
// tf = lowison tf=0 lowison=0 (high is on), tf=1 lowison=1 (low is on)
//=============================================================================
    constexpr   Pins::Pins          (RPN e, bool tf)
//=============================================================================
    : m_pt((volatile uint32_t*)ANSELA + ((e>>RN_SHIFT)/PINMAX)*ANSELX_SPACING),
      m_pn(1<<((e>>RN_SHIFT)%PINMAX)),
      m_lowison(tf),
      m_rpn((uint8_t)e & 31),
      m_ppsin(PPSINOFF)
{
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