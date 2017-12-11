#include "Pins.hpp"
#include "Syskey.hpp"

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



/*=============================================================================
 Pins functions - static
=============================================================================*/
//unlock, write byte, lock
void Pins::pps_do( uint32_t r, uint8_t v )
{
    Syskey::unlock();
    Reg::clr( RPCON, IOLOCK );
    Reg::val8( r, v );
    Reg::set( RPCON, IOLOCK );
    Syskey::lock();
}
//pps off for peripheral
void Pins::pps_off( PPSIN e ){          pps_in( e, (RPN)0 ); }
void Pins::pps_in( PPSIN e, RPN n )
{
    pps_do( RPINR1+((e/4)*16)+(e%4), n&31 );
    Reg::set( ANSELA + TRIS + ((n>>8)/16)*0x100, 1<<((n>>8)%16) );  //tris=1
    Reg::clr( ANSELA + ((n>>8)/16)*0x100, 1<<((n>>8)%16) );         //ansel=0
}
//pin output not using pps
void Pins::pps_off( RPN n ){            pps_out( PPSOFF, n ); }
void Pins::pps_out( PPSOUT e, RPN n )
{
    pps_do( RPOR0+((((n&31)-1)/4)*16)+(((n&31)-1)%4), e );
}

