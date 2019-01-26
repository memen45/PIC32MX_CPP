#include "UsbDebug.hpp"
#include <cstdio>  //printf

//_mon_putc has to be used so printf can go out our uart
//currently created 'somehwere else'


static const int bufsize{128};
static const int bufn{2};
static char m_buf[bufn][bufsize]{0};
static bool m_enable{true};

// enable/disable
//=============================================================================
            auto UsbDebug::
debug       (bool tf) -> void
            {
            m_enable = tf;
            }

// get enable/disable state
//=============================================================================
            auto UsbDebug::
debug       () -> bool
            {
            return m_enable;
            }

// get a buffer to use
//=============================================================================
            auto UsbDebug::
getbuf      (int* siz) -> char*
            {
            *siz = 0;
            if( not m_enable ) return (char*)0;
            for(auto& b : m_buf){
                if( b[0] ) continue; //in use if not 0
                b[0] = ' '; //mark as in use (harmless)
                *siz = bufsize; //provide size
                return &b[0];
            }
            return (char*)0;
            }


// send buffer contents out via printf
//=============================================================================
            auto UsbDebug::
debug       (char* msg) -> void
            {
            if( m_enable ) printf( "%s", msg );
            msg[0] = 0; //mark buffer as available
            }
