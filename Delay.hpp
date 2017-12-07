#pragma once

/*=============================================================================
 Delay using Coprocessor0 Count
=============================================================================*/

#include <cstdint>

class DelayCP0 {

    public:

        DelayCP0( uint8_t cpu_MHz = 24 );

        bool expired( void );
        void reset( void );
        void wait_us( uint32_t );
        void wait_ms( uint32_t );
        void set_us( uint32_t );
        void set_ms( uint32_t );

    private:

        void reset( uint32_t );
        void wait( uint32_t );
        uint32_t m_start;
        uint32_t m_countn;
        uint8_t m_cpu_MHz;
        bool m_expired;
};

