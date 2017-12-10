#pragma once

/*=============================================================================
 CLC1-4 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Clc {

    public:

        enum CLC
        {
            CLC1 = 0xBF802480,
            CLC2 = 0xBF802500,
            CLC3 = 0xBF802580,
            CLC4 = 0xBF802600
        };

        enum MODE : uint8_t
        {
            ANDOR = 0, ORXOR = 1, AND = 2, SR = 3,
            DSR = 4, DR = 5, JKR = 6, LSR = 7
        };
        enum GXPOL { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };


        constexpr Clc( CLC );

        void gate_inv( GXPOL, bool );
        void on( bool );
        void stop_idle( bool );
        void intp( bool );
        void intn( bool );
        void out( bool );
        bool out( void );
        void out_inv( bool );
        void mode( MODE e );
        void in_sel( uint8_t, uint8_t );
        void in_sel( uint32_t );
        void gate_sel( uint8_t, uint8_t );
        void gate_sel( uint32_t );


    private:

        enum
        {
            SEL_OFFSET = 4, GLS_OFFSET = 8, //offsets in words
            ON = 1<<15, SIDL = 1<<13, INTP = 1<<11 , INTN = 1<<10,
            LCOE = 1<<7, LCOUT = 1<<6, LCPOL = 1<<5
        };

        volatile uint32_t * m_pt;
};


/*=============================================================================
 all functions inline
=============================================================================*/

constexpr Clc::Clc( CLC n ) :
    m_pt( (volatile uint32_t*)n )
{
}

void Clc::gate_inv( GXPOL e, bool tf ){      Reg::set( m_pt, e, tf ); }
void Clc::on( bool tf ){                     Reg::set( m_pt, ON, tf ); }
void Clc::stop_idle( bool tf ){              Reg::set( m_pt, SIDL, tf ); }
void Clc::intp( bool tf ){                   Reg::set( m_pt, INTP, tf ); }
void Clc::intn( bool tf ){                   Reg::set( m_pt, INTN, tf ); }
void Clc::out( bool tf ){                    Reg::set( m_pt, LCOE, tf ); }
bool Clc::out( void ){                return Reg::is_set( m_pt, LCOUT ); }
void Clc::out_inv( bool tf ){                Reg::set( m_pt, LCPOL, tf ); }
void Clc::mode( MODE e ){    Reg::clr( m_pt, LSR ); Reg::set( m_pt, e ); }

//input select, dsn = 1-4, val = 0-7 (invalid args masked to good vals)
void Clc::in_sel( uint8_t dsn, uint8_t val )
{
    dsn -= 1; dsn &= 3; dsn <<= 2; val &= 7;
    Reg::clr( m_pt+SEL_OFFSET, 7<<dsn );
    Reg::set( m_pt+SEL_OFFSET, val<<dsn );
}
//or all in in shot
void Clc::in_sel( uint32_t val ){      Reg::val( m_pt+SEL_OFFSET, val ); }

//gate select, gate = 1-4 (invalid gate masked to good gate)
void Clc::gate_sel( uint8_t gate, uint8_t val )
{
    gate -= 1; gate &= 3; gate <<= 3;
    Reg::clr( m_pt+GLS_OFFSET, 15<<gate );
    Reg::set( m_pt+GLS_OFFSET, val<<gate );
}
//or all in in shot
void Clc::gate_sel( uint32_t val ){      Reg::val( m_pt+GLS_OFFSET, val ); }
