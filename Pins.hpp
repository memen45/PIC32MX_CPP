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

        constexpr Pins( PORT, uint8_t, bool = false );
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
        void pps_in( PPSIN, bool = true );
        void pps_out( PPSOUT );


    private:

        enum //offsets from base address, in words (>>2)
        {
            TRIS = 16>>2, PORT_ = 32>>2, LAT = 48>>2, ODC = 64>>2,
            CNPU = 80>>2, CNPD = 96>>2, CNCON = 112>>2, CNEN0 = 128>>2,
            CNSTAT = 144>>2, CNEN1 = 160>>2, CNF = 176>>2
        };

        enum { ON = 1<<15, CNSTYLE = 1<<11 };

        enum
        {
            RPCON = 0xBF802A00, IOLOCK = 1<<11,
            RPINR1 = 0xBF802A10,
            RPOR0 = 0xBF802B10,
        };

        void pps_do( uint32_t, uint8_t );

        const uint16_t m_pn;        //pin mask
        volatile uint32_t* m_pt;    //base address
        bool m_lowison;             //pin on val is low
        uint8_t m_rpn;              //RPn number for pps (0=no Rpn for pin)

        static const uint8_t rpn_map[]; //RXn -> RPn map
};

/*=============================================================================
 inline functions
=============================================================================*/
constexpr Pins::Pins( PORT e, uint8_t pn, bool lowison ) :
     m_pt( (volatile uint32_t*)e ),
     m_pn( 1<<(pn&15) ),
     m_lowison( lowison ),
     m_rpn(
        e == A ? rpn_map[pn&15] :
            e == B ? rpn_map[(pn&15)+16] :
                e == C ? rpn_map[(pn&15)+32] : 0
     )
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