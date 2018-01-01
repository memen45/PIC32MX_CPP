#pragma once

/*=============================================================================
 I/O pin functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Pins {

    public:

    //instantiate Pins with the following pin name
    enum PORTPIN : uint8_t {
        A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
        B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B13= 29,B14,B15,
        C0,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,C13,C14,C15,
        D0,D1,D2,D3,D4
    };
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
        CLCINA = 42, CLCINB = 43                    //R12
    };
    //pps_out peripheral names
    enum PPSOUT : uint8_t {
        PPSOFF = 0,
        C1OUT, C2OUT, C3OUT,
        U2TX, U2RTS, U3TX, U3RTS,
        SDO2, SCK2OUT, SS2OUT,
        OCM4, OCM5, OCM6, OCM7, OCM8, OCM9,
        CLC1OUT, CLC2OUT, CLC3OUT, CLC4OUT,
    };
    //pps_in/pps_out pin names (either RPn or RXn)
    //(can also instantiate Pins with these names)
    enum RPN : uint16_t {
        // encode as- 0x000ppnnnn000rrrrr - | 000 PORT/PIN | 000 RPn |
        // pp = port A=0,B=1,C=2,D=3
        // nnnnn = pin = 0-15
        // rrrrr = RPn = 1-24
        // so we can use one enum to get both RPn and port/pin info
        // (can then also set tris, clear ansel, for pps_in, and also
        //  use RPn or RXn as init value for Pins() in addition to Xn)

        //RPn 1-24
        RP1 = 1+(A0<<8), RP2 = 2+(A1<<8), RP3 = 3+(A2<<8), RP4 = 4+(A3<<8),
        RP5 = 5+(A4<<8), RP6 = 6+(B0<<8), RP7 = 7+(B1<<8), RP8 = 8+(B2<<8),
        RP9 = 9+(B3<<8), RP10 = 10+(B4<<8), RP11 = 11+(B5<<8),
        RP12 = 12+(B7<<8), RP13 = 13+(B8<<8), RP14 = 14+(B9<<8),
        RP15 = 15+(B13<<8), RP16 = 16+(B14<<8), RP17 = 17+(B15<<8),
        RP18 = 18+(C8<<8), RP19 = 19+(C2<<8), RP20 = 20+(C7<<8),
        RP21 = 21+(A7<<8), RP22 = 22+(A10<<8), RP23 = 23+(C6<<8),
        RP24 = 24+(A9<<8),

        //RXn to RPn
        RA0 = RP1, RA1 = RP2, RA2 = RP3, RA3 = RP4, RA4 = RP5, RB0 = RP6,
        RB1 = RP7, RB2 = RP8, RB3 = RP9, RB4 = RP10, RB5 = RP11, RB7 = RP12,
        RB8 = RP13, RB9 = RP14, RB13 = RP15, RB14 = RP16, RB15 = RP17,
        RC8 = RP18, RC2 = RP19, RC7 = RP20, RA7 = RP21, RA10 = RP22,
        RC6 = RP23, RA9 = RP24
    };

    //public functions
    //r/w pins
    bool        pinval          () const;
    bool        latval          () const;
    void        low             () const;
    void        high            () const;
    void        invert          () const;
    void        on              (bool) const;
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
    void        icn_flagclear   () const;
    //pps
    static void pps_off         (PPSIN);
    static void pps_in          (PPSIN, RPN);
    static void pps_off         (RPN);
    static void pps_out         (PPSOUT, RPN);

    //constructors (bool-> lowison)
    constexpr   Pins            (PORTPIN, bool = false); //A0, B3, C9, etc.
    constexpr   Pins            (RPN, bool = false); //or RP8, RB2, etc.

    private:

    static Reg r;

    enum { //offsets from base address, in words
        TRIS = 16>>2, PORT_ = 32>>2, LAT = 48>>2, ODC = 64>>2,
        CNPU = 80>>2, CNPD = 96>>2, CNCON = 112>>2, CNEN0 = 128>>2,
        CNSTAT = 144>>2, CNEN1 = 160>>2, CNF = 176>>2
    };

    enum {
        ANSELA = 0xBF802BB0, ANSELX_SPACING = 64, //spacing in words
        ON = 1<<15, CNSTYLE = 1<<11,
        RPCON = 0xBF802A00, IOLOCK = 1<<11,
        RPINR1 = 0xBF802A10,
        RPOR0 = 0xBF802B10,
        RPN_MASK = 7, RN_SHIFT = 8, PINMAX = 16
    };

    //private functions
    static void     pps_do      (uint32_t, uint8_t);

    //private vars
    const uint16_t      m_pn;       //pin mask
    volatile uint32_t*  m_pt;       //base address
    bool                m_lowison;  //pin on val is low
};

/*=============================================================================
 inline functions
=============================================================================*/

// A0 format - Pins led1(A0); or Pins led2(B3, true);
constexpr Pins::Pins(PORTPIN e, bool lowison)
    : m_pt((volatile uint32_t*)ANSELA + (e/PINMAX)*ANSELX_SPACING),
      m_pn(1<<(e%PINMAX)),
      m_lowison(lowison)
{}
// RA0/RP1 format - Pins led1(RA0); or Pins led2(RP1, true);
constexpr Pins::Pins(RPN e, bool lowison)
    : m_pt((volatile uint32_t*)ANSELA + ((e>>RN_SHIFT)/PINMAX)*ANSELX_SPACING),
      m_pn(1<<((e>>RN_SHIFT)%PINMAX)),
      m_lowison(lowison)
{}

bool Pins::pinval() const {
    return r.anybit(m_pt+PORT_, m_pn);
}
bool Pins::latval() const {
    return r.anybit(m_pt+LAT, m_pn);
}
void Pins::low() const {
    r.setbit(m_pt+LAT, m_pn, 0);
}
void Pins::high() const {
    r.setbit(m_pt+LAT, m_pn);
}
void Pins::invert() const {
    r.flipbit(m_pt+LAT, m_pn);
}
void Pins::on(bool tf) const {
    r.setbit(m_pt+LAT, m_pn, tf^m_lowison);
}
bool Pins::ison() const {
    return m_lowison ? !pinval() : pinval();
}
void Pins::icn_flagclear() const {
    r.setbit(m_pt+CNF, m_pn, 0);
}