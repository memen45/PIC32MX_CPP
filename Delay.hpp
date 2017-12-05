#ifndef _DELAY_H
#define _DELAY_H

#include <cstdint>
#include "Cp0.hpp"

class DelayCP0 {

    public:
        DelayCP0( uint8_t cpu_MHz = 24 );

        bool isexpired( void );
        void reset( void );
        void reset( uint32_t );
        void wait( uint32_t );
        void wait_us( uint32_t );
        void wait_ms( uint32_t );
        void set_us( uint32_t );
        void set_ms( uint32_t );

    private:
        uint32_t m_start;
        uint32_t m_countn;
        uint8_t m_cpu_MHz;
        bool m_expired;
};

#endif /* _DELAY_H */
