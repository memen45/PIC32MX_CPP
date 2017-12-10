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
            ANDOR = 0, ORXOR = 1, AND = 2, SR = 3, DSR = 4,
            DR = 5, JKR = 6, LSR = 7
        };

    private:

        enum
        {
            SEL_OFFSET = 4, GLS_OFFSET = 8, //offsets in words
            ON = 1<<15, SIDL = 1<<13, INTP = 1<<11 , INTN = 1<<10,
            LCOE = 1<<7, LCOUT = 1<<6, LCPOL = 1<<5
        };

        enum GXPOL
        {
            G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16
        };

    public:

        constexpr Clc( CLC n ) :
            m_pt( (volatile uint32_t*)n )
        {
        }

        void gate_inv( GXPOL e, bool tf ){      Reg::set( m_pt, e, tf ); }
        void on( bool tf ){                     Reg::set( m_pt, ON, tf ); }
        void stop_idle( bool tf ){              Reg::set( m_pt, SIDL, tf ); }
        void intp( bool tf ){                   Reg::set( m_pt, INTP, tf ); }
        void intn( bool tf ){                   Reg::set( m_pt, INTN, tf ); }
        void out( bool tf ){                    Reg::set( m_pt, LCOE, tf ); }
        bool out( void ){                return Reg::is_set( m_pt, LCOUT ); }
        void out_inv( bool tf ){                Reg::set( m_pt, LCPOL, tf ); }
        void mode( MODE e ){    Reg::clr( m_pt, LSR ); Reg::set( m_pt, e ); }

        //input select, dsn = 1-4, val = 0-7 (invalid args masked to good vals)
        void in_sel( uint8_t dsn, uint8_t val )
        {
            dsn -= 1; dsn &= 3; dsn <<= 2; val &= 7;
            Reg::clr( m_pt+SEL_OFFSET, 7<<dsn );
            Reg::set( m_pt+SEL_OFFSET, val<<dsn );
        }
        //or all in in shot
        void in_sel( uint32_t val ){      Reg::val( m_pt+SEL_OFFSET, val ); }

        //gate select, gate = 1-4 (invalid gate masked to good gate)
        void gate_sel( uint8_t gate, uint8_t val )
        {
            gate -= 1; gate &= 3; gate <<= 3;
            Reg::clr( m_pt+GLS_OFFSET, 15<<gate );
            Reg::set( m_pt+GLS_OFFSET, val<<gate );
        }
        //or all in in shot
        void gate_sel( uint32_t val ){      Reg::val( m_pt+GLS_OFFSET, val ); }


    private:

        volatile uint32_t * m_pt;
};

