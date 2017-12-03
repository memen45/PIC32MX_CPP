#ifndef _DELAY_H
#define _DELAY_H

#include <cstdint>
#define CP0_GET_COUNT() __builtin_mfc0(9,0)

class DelayCP0 {

    public:
        //pass in cpu freq in MHz, default is 24
        DelayCP0( uint8_t cpu_mhz = 24 ) :
            m_cpu_MHz(cpu_mhz),
            m_countn(0),
            m_expired(true)
        {
        }

        bool isexpired()
        {
            if( (CP0_GET_COUNT() - m_start) >= m_countn ) m_expired = true;
            return m_expired;
        }
        void reset(){ m_start = CP0_GET_COUNT(); m_expired = false; }
        void reset( uint32_t n ){   reset(); m_countn = n>>1; }
        void wait( uint32_t n ){    reset( n ); while( !isexpired() ); }
        void wait_us( uint32_t n ){ wait( m_cpu_MHz * n ); }
        void wait_ms( uint32_t n ){ wait_us( 1000 * n ); }
        void set_us( uint32_t n ){  reset( m_cpu_MHz * n ); }
        void set_ms( uint32_t n ){  set_us( 1000 * n ); }

    private:
        uint32_t m_start;
        uint32_t m_countn;
        uint8_t m_cpu_MHz;
        bool m_expired;
};

#endif /* _DELAY_H */
