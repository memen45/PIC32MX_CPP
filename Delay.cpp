#include "Delay.hpp"
#include "Cp0.hpp"

/*=============================================================================
 DelayCP0 functions
=============================================================================*/

DelayCP0::DelayCP0( uint8_t cpu_mhz ) :
    m_cpu_MHz( cpu_mhz ),
    m_countn( 0 ),
    m_expired( true )
{
}

bool DelayCP0::expired( void )
{
    if( (Cp0::count() - m_start) >= m_countn) m_expired = true;
    return m_expired;
}

void DelayCP0::reset( void )
{
    m_start = Cp0::count();
    m_expired = false;
}

void DelayCP0::wait_us( uint32_t n ){     wait( m_cpu_MHz * n ); }
void DelayCP0::wait_ms( uint32_t n ){     wait_us( 1000 * n ); }
void DelayCP0::set_us( uint32_t n ){      reset( m_cpu_MHz * n ); }
void DelayCP0::set_ms( uint32_t n ){      set_us( 1000 * n ); }

//private
void DelayCP0::reset( uint32_t n ){       reset(); m_countn = n>>1; }
void DelayCP0::wait( uint32_t n ){        reset( n ); while( ! expired() ); }