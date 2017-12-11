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


void Pins::odrain( bool tf ){           Reg::set( m_pt+ODC, m_pn, tf ); }
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

//pps
void Pins::pps_in( PPSIN e )
{
    //if pin not an RPn, peripheral input will be disabled (0)
    Reg::val8( RPINR1+(e/4)*16, m_pn<<(e%4) );
}
void Pins::pps_out( PPSOUT e )
{
    if( m_rpn == 0 ) return; //not an RPn pin
    m_rpn -= 1; //rpn 1 based, to 0 based for reg offset calc
    Reg::val8( RPOR0+(m_rpn/4)*16, e<<(m_rpn%4) );
}

// RXn to RPn map
const uint8_t Pins::rpn_map[] =
{
    //RA0-RA15 -RPx (0=noRp for pin)
    1,2,3,4,5,0,0,21,0,24,22,0,0,0,0,0,
    //RB0-15 -RPx
    6,7,8,9,10,11,0,12,13,14,0,0,0,15,16,17,
    //RC0-15
    0,0,19,0,0,0,23,20,18,0,0,0,0,0,0
};
