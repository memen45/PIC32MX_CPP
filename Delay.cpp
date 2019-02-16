#include "Delay.hpp"
#include "Cp0.hpp"
#include "Osc.hpp"

//Delay functions

// set us
//=============================================================================
            Delay::
Delay       (uint32_t us)
            {
            set(us);
            }

//=============================================================================
            auto Delay::
expired     () -> bool
            {
            if( m_expired ) return true; //do not calc, already know
            if((Cp0::count() - m_start) >= m_countn) expire();
            return m_expired;
            }

//force time expired
//=============================================================================
            auto Delay::
expire      () -> void
            {
            m_expired = true;
            }

//=============================================================================
            auto Delay::
restart     () -> void
            {
            m_start = Cp0::count();
            m_expired = false;
            }

// in micrososends
//=============================================================================
            auto Delay::
set         (uint32_t n) -> void
            {
            if(n > m_max_us) n = m_max_us;
            set_count(n);
            }

// private
//=============================================================================
            auto Delay::
set_count   (uint32_t n) -> void
            {
            restart();
            m_countn = Osc::sysclk() / 2000000UL * n;
            }

// static
// in micrososends
//=============================================================================
            auto Delay::
wait        (uint32_t n) -> void
            {
            if(n > m_max_us) n = m_max_us;
            n = Osc::sysclk() / 2000000UL * n;
            uint32_t start = Cp0::count();
            while((Cp0::count() - start) < n);
            }
