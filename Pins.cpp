#include "Pins.hpp"

/*=============================================================================
 Pins functions
=============================================================================*/

void Pins::lowison( bool tf ){          m_lowison = tf; }

void Pins::digital_in( void )
{
    Reg::set( m_pt+TRIS, m_pn );        //tris=1
    Reg::clr( m_pt, m_pn );             //ansel=0
}
void Pins::analog_in( void )
{
    Reg::set( m_pt+TRIS, m_pn );        //tris=1
    Reg::set( m_pt, m_pn );             //ansel=1
}
void Pins::digital_out( void )
{
    Reg::clr( m_pt+TRIS, m_pn );        //tris=0
    Reg::clr( m_pt, m_pn );             //ansel=0
}


void Pins::odc( bool tf ){              Reg::set( m_pt+ODC, m_pn, tf ); }
void Pins::pullup( bool tf ){           Reg::set( m_pt+CNPU, m_pn, tf ); }
void Pins::pulldn( bool tf ){           Reg::set( m_pt+CNPD, m_pn, tf ); }
void Pins::icn( bool tf ){              Reg::set( m_pt+CNCON, ON, tf ); }

void Pins::icn_rising( void ){
    Reg::set( m_pt+CNCON, CNSTYLE );
    Reg::set( m_pt+CNEN0, m_pn );
    Reg::clr( m_pt+CNEN1, m_pn );
}
void Pins::icn_risefall( void ){
    Reg::set( m_pt+CNCON, CNSTYLE );
    Reg::set( m_pt+CNEN0, m_pn );
    Reg::clr( m_pt+CNEN1, m_pn );
}
void Pins::icn_falling( void ){
    Reg::set( m_pt+CNCON, CNSTYLE );
    Reg::set( m_pt+CNEN1, m_pn );
    Reg::clr( m_pt+CNEN0, m_pn );
}
void Pins::icn_mismatch( void ){
    Reg::set( m_pt+CNEN0, m_pn );
    Reg::clr( m_pt+CNCON, CNSTYLE );
}

bool Pins::icn_flag( void ){        return Reg::is_set( m_pt+CNF, m_pn ); }
bool Pins::icn_stat( void ){        return Reg::is_set( m_pt+CNSTAT, m_pn ); }

