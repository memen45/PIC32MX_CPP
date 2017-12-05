#include "Pins.hpp"

void Pins::lowison( void ){             m_lowison = true; }
void Pins::lowisoff( void ){            m_lowison = false; }
void Pins::digital( void ){             Reg::clr( m_pt, m_pn ); }
void Pins::analog( void ){              Reg::set( m_pt, m_pn ); }
void Pins::output( void ){
    digital();                          //implied, cannot have analog output
    Reg::clr( m_pt+4, m_pn );
}
void Pins::input( void ){               Reg::set( m_pt+4, m_pn ); }

void Pins::odc_off( void ){             Reg::clr( m_pt+16, m_pn ); }
void Pins::odc_on( void ){              Reg::set( m_pt+16, m_pn ); }
void Pins::pullup_off( void ){          Reg::clr( m_pt+20, m_pn ); }
void Pins::pullup_on( void ){           Reg::set( m_pt+20, m_pn ); }
void Pins::pulldn_off( void ){          Reg::clr( m_pt+24, m_pn ); }
void Pins::pulldn_on( void ){           Reg::set( m_pt+24, m_pn ); }

void Pins::icnoff( void ){              Reg::clr( m_pt+28, 1<<15 ); }
void Pins::icnon( void ){               Reg::set( m_pt+28, 1<<15 ); }
void Pins::icnmatch( void ){            Reg::clr( m_pt+28, 1<<11 ); }
void Pins::icnedge( void ){             Reg::set( m_pt+28, 1<<11 ); }
void Pins::icnrisingoff( void ){        Reg::clr( m_pt+32, m_pn ); }
void Pins::icnrising( void ){           icnedge(); Reg::set( m_pt+32, m_pn ); }
void Pins::icnfallingoff( void ){       Reg::clr( m_pt+40, m_pn ); }
void Pins::icnfalling( void ){          icnedge(); Reg::set( m_pt+40, m_pn ); }
void Pins::icnmismatch( void ){         icnfalling(); icnmatch(); }
bool Pins::icnflag( void ){             return Reg::is_set( m_pt+44, m_pn ); }
bool Pins::icnstat( void ){             return Reg::is_set( m_pt+36, m_pn ); }

