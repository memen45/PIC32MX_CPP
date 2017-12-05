#ifndef _TIMER23_H
#define _TIMER23_H

#include <cstdint>
#include "Reg.hpp"

class Timer23 {

    public:
        enum TMR23 : uint32_t {
            T2 = 0xBF808040,
            T3 = 0xBF808080
        };

    private:
        enum {
            TMRX = 4,
            PRX = 8,
            ON = 1<<15, SIDL = 1<<13,
            EXT_RES = 3<<8, EXT_LPRC = 2<<8, EXT_T1CK = 1<<8, EXT_SOSC = 0<<8,
            TGATE = 1<<7,
            TCKPS_256 = 7<<4, TCKPS_64 = 6<<4, TCKPS_32 = 5<<4, TCKPS_16 = 4<<4,
            TCKPS_8 = 3<<4,   TCKPS_4 = 2<<4,  TCKPS_2 = 1<<4,  TCKPS_1 = 0<<4,
            T32 = 1<<3,
            TCS = 1<<1
        };

    public:
        Timer23( TMR23 tn ) :
            m_pt( (volatile uint32_t*)tn )
        {
        };

        void timer( uint16_t n ){       Reg::val( m_pt + TMRX, n ); }
        uint16_t timer( void ){         return Reg::val( m_pt + TMRX ); }

        void pr( uint16_t n ){          Reg::val( m_pt + PRX, n ); }
        uint16_t pr( void ){            return Reg::val( m_pt + PRX ); }

        void on( void ){                Reg::set( m_pt, ON ); }
        void off( void ){               Reg::clr( m_pt, ON ); }

        void stop_idle( void ){         Reg::set( m_pt, SIDL ); }
        void cont_idle( void ){         Reg::clr( m_pt, SIDL ); }

        void tgate_on( void ){          Reg::set( m_pt, TGATE ); }
        void tgate_off( void ){         Reg::clr( m_pt, TGATE ); }

        void pre_256( void ){           Reg::set( m_pt, TCKPS_256 ); }
        void pre_64( void ){            pre_1(); Reg::set( m_pt, TCKPS_64 ); }
        void pre_32( void ){            pre_1(); Reg::set( m_pt, TCKPS_32 ); }
        void pre_16( void ){            pre_1(); Reg::set( m_pt, TCKPS_16 ); }
        void pre_8( void ){             pre_1(); Reg::set( m_pt, TCKPS_8 ); }
        void pre_4( void ){             pre_1(); Reg::set( m_pt, TCKPS_4 ); }
        void pre_2( void ){             pre_1(); Reg::set( m_pt, TCKPS_2 ); }
        void pre_1( void ){             Reg::clr( m_pt, TCKPS_256 ); }

        void t32bit( void ){            Reg::set( m_pt, T32 ); }
        void t16bit( void ){            Reg::clr( m_pt, T32 ); }

        void ext_clk( void ){           Reg::set( m_pt, TCS ); }
        void int_clk( void ){           Reg::clr( m_pt, TCS ); }



    private:
        volatile uint32_t * m_pt;
};

#endif //_TIMER1_H
