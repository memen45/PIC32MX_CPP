#include "UsbDebug.hpp"
#include <cstdio>  //printf
#include "Cp0.hpp"

//_mon_putc has to be used so printf can go out our uart
//currently created 'somehwere else'

//debug is a blocking function

static bool m_enable{true};

char UsbDebug::buffer[UsbDebug::bufsiz] = {0};

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

// send buffer contents out via printf
// (filename, funcname, message)
//=============================================================================
            auto UsbDebug::
debug       (const char* fil, const char* fnam) -> void
            {
            if( not m_enable ) return;
            printf( "[$1%010u$7]", Cp0::count() );
            printf( "[$2%-14s$7]", fil ); //filename
            printf( "[$5%-14s$7] %s\r\n", fnam, buffer );
            }



