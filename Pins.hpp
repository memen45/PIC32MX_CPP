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


        enum //offsets from base address, in words (>>4)
        {
            TRIS = 16>>2, PORT_ = 32>>2, LAT = 48>>2, ODC = 64>>2,
            CNPU = 80>>2, CNPD = 96>>2, CNCON = 112>>2, CNEN0 = 128>>2,
            CNSTAT = 144>>2, CNEN1 = 160>>2, CNF = 176>>2
        };

        enum { ON = 1<<15, CNSTYLE = 1<<11 };


        //inline
        constexpr Pins( PORT e, uint8_t pn, bool lowison = false ) :
             m_pt( (volatile uint32_t*)e ),
             m_pn( 1<<pn ),
             m_lowison( lowison )
        {
        }

        //inline
        bool pinval( void ){        return Reg::is_set( m_pt+PORT_, m_pn ); }
        bool latval( void ){        return Reg::is_set( m_pt+LAT, m_pn ); }
        void low( void ){           Reg::clr( m_pt+LAT, m_pn ); }
        void high( void ){          Reg::set( m_pt+LAT, m_pn ); }
        void invert( void ){        Reg::inv( m_pt+LAT, m_pn ); }
        void on( void ){            m_lowison ? low() : high(); }
        void off( void ){           m_lowison ? high() : low(); }
        bool ison( void ){          return m_lowison ? !pinval() : pinval(); }
        bool isoff( void ){         return !ison(); }

        //cpp
        void lowison( bool );
        void digital_in( void );
        void analog_in( void );
        void digital_out( void );
        void odrain( bool );
        void pullup( bool );
        void pulldn( bool );

        //input change notification
        //cpp
        void icn( bool );
        void icn_rising( void );
        void icn_falling( void );
        void icn_risefall( void );
        void icn_mismatch( void );
        bool icn_flag( void );
        bool icn_stat( void );

        //inline
        void icn_flagclear( void ){     Reg::clr( m_pt+CNF, m_pn ); }

    private:

        const uint16_t m_pn;        //pin mask
        volatile uint32_t* m_pt;    //base address
        bool m_lowison;             //pin on val is low
};

