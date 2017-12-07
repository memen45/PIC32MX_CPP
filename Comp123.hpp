#pragma once

/*=============================================================================
 Compare1/2/3 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

class Comp123 {

    public:

        enum CMP123 : uint32_t
        {
            C1 = 0xBF802310,
            C2 = 0xBF802320,
            C3 = 0xBF802330
        };

    private:

        enum
        {
            CMSTAT = 0xBF802300,
            SIDL = 1<<13, CVREFSEL = 1<<8,
            ON = 1<<15, COE = 1<<14, CPOL = 1<<13, CEVT = 1<<9, COUT = 1<<8
        };

        enum EVTPOL
        {
            ANY = 3<<6, HL = 2<<6, LH = 1<<6, OFF = 0,
        };

        enum CCH
        {
            BGAP = 3, CXIND = 2, CXINC = 1, CXINB = 0
        }

    public:

        Comp123( CMP123 n ) :
            m_pt( n )
        {
        };

        //all instances can write to cmstat
        void stop_idle( void ){         Reg::set( CMSTAT, SIDL ); }
        void cont_idle( void ){         Reg::clr( CMSTAT, SIDL ); }
        void ref_ext( void ){           Reg::set( CMSTAT, CVREFSEL ); }
        void ref_int( void ){           Reg::clr( CMSTAT, CVREFSEL ); }

        void on( void ){                Reg::set( m_pt, ON ); }
        void off( void ){               Reg::clr( m_pt, ON ); }
        void out_on( void ){            Reg::set( m_pt, COE ); }
        void out_off( void ){           Reg::clr( m_pt, COE ); }
        void out_inv( void ){           Reg::set( m_pt, CPOL ); }
        void out_norm( void ){          Reg::clr( m_pt, CPOL ); }
        bool event_bit( void ){         return Reg::is_set( m_pt, CEVT ); }
        bool out_bit( void ){           return Reg::is_set( m_pt, COUT ); }
        void event_sel( EVTPOL p )
        {
            Reg::clr( m_pt, ANY );
            Reg::set( m_pt, p );
        }
        void cref_refsel( void ){       Reg::set( m_pt, CREF ); }
        void cref_cxina( void ){        Reg::clr( m_pt, CREF ); }
        void ch_sel( CCH c )
        {
            Reg::clr( m_pt, BGAP );
            Reg::set( m_pt, c );
        }

    private:

        volatile uint32_t * m_pt;
};

