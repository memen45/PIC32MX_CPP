#pragma once

/*=============================================================================
 I/O pin functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Pins {

    public:

        enum PORT : uint32_t
        {
            A = 0xBF802BB0, //make ANSELA base addr
            B = 0xBF802CB0, //make ANSELB base addr
            C = 0xBF802DB0, //make ANSELC base addr
            D = 0xBF802EB0, //make ANSELD base addr
        };

        enum PORTPIN : uint8_t
        {
            A0 = 0, A1, A2, A3, A4, A5, A6, A7, A8,
            A9, A10, A11, A12, A13, A14, A15,

            B0, B1, B2, B3, B4, B5, B6, B7, B8,
            B9, B10, B11,   B13 = 29, B14, B15,

            C0, C1, C2, C3, C4, C5, C6, C7, C8,
            C9, C10, C11, C12, C13, C14, C15,

            D0, D1, D2, D3, D4
        };

        enum PPSIN //byte offset from RPINR1
        {
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

        enum PPSOUT : uint8_t
        {
            PPSOFF = 0,
            C1OUT, C2OUT, C3OUT,
            U2TX, U2RTS, U3TX, U3RTS,
            SDO2, SCK2OUT, SS2OUT,
            OCM4, OCM5, OCM6, OCM7, OCM8, OCM9,
            CLC1OUT, CLC2OUT, CLC3OUT, CLC4OUT,
        };

        enum RPN : uint8_t
        {
            //RPn 1-24
            RP1 = 1, RP2, RP3, RP4, RP5, RP6, RP7, RP8, RP9, RP10, RP11,
            RP12, RP13, RP14, RP15, RP16, RP17, RP18, RP19, RP20, RP21,
            RP22, RP23, RP24,

            //RXn to RPn
            RA0 = 1, RA1, RA2, RA3, RA4, RA7 = 21, RA9 = 24, RA10 = 22,
            RB0 = 6, RB1, RB2, RB3, RB4, RB5, RB7 = 12, RB8, RB9,
            RB13 = 15, RB14, RB15, RC2 = 19, RC6 = 23, RC7 = 20, RC8 = 18
        };

        constexpr Pins( PORTPIN, bool = false );
        bool pinval( void );
        bool latval( void );
        void low( void );
        void high( void );
        void invert( void );
        void on( void );
        void off( void );
        bool ison( void );
        bool isoff( void );
        void lowison( bool );
        void digital_in( void );
        void analog_in( void );
        void digital_out( void );
        void odrain( bool );
        void pullup( bool );
        void pulldn( bool );
        void icn( bool );
        void icn_rising( void );
        void icn_falling( void );
        void icn_risefall( void );
        void icn_mismatch( void );
        bool icn_flag( void );
        bool icn_stat( void );
        void icn_flagclear( void );

        //pps
        static void pps_off( PPSIN );
        static void pps_in( PPSIN, RPN );
        static void pps_off( RPN );
        static void pps_out( PPSOUT, RPN );


    private:

        enum //offsets from base address, in bytes
        {
            TRIS = 16, PORT_ = 32, LAT = 48, ODC = 64,
            CNPU = 80, CNPD = 96, CNCON = 112, CNEN0 = 128,
            CNSTAT = 144, CNEN1 = 160, CNF = 176
        };

        enum { ON = 1<<15, CNSTYLE = 1<<11 };

        enum
        {
            ANSELA = 0xBF802BB0,
            RPCON = 0xBF802A00, IOLOCK = 1<<11,
            RPINR1 = 0xBF802A10,
            RPOR0 = 0xBF802B10,
        };

        static void pps_do( uint32_t, uint8_t );

        const uint16_t m_pn;        //pin mask
        uint32_t m_pt;              //base address
        bool m_lowison;             //pin on val is low
};

/*=============================================================================
 inline functions
=============================================================================*/
constexpr Pins::Pins( PORTPIN e, bool lowison ) :
     m_pt( ANSELA + (e/16)*0x100 ),
     m_pn( 1<<(e%16) ),
     m_lowison( lowison )
{
}

bool Pins::pinval( void ){        return Reg::is_set( m_pt+PORT_, m_pn ); }
bool Pins::latval( void ){        return Reg::is_set( m_pt+LAT, m_pn ); }
void Pins::low( void ){           Reg::clr( m_pt+LAT, m_pn ); }
void Pins::high( void ){          Reg::set( m_pt+LAT, m_pn ); }
void Pins::invert( void ){        Reg::inv( m_pt+LAT, m_pn ); }
void Pins::on( void ){            m_lowison ? low() : high(); }
void Pins::off( void ){           m_lowison ? high() : low(); }
bool Pins::ison( void ){          return m_lowison ? !pinval() : pinval(); }
bool Pins::isoff( void ){         return !ison(); }
void Pins::icn_flagclear( void ){ Reg::clr( m_pt+CNF, m_pn ); }