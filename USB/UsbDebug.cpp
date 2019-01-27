#include "UsbDebug.hpp"
#include <cstdio>  //printf
#include "Cp0.hpp"
#include <cstdarg> //variadic args

#include <ostream>

//_mon_putc has to be used so printf can go out our uart
//currently created 'somehwere else'

//debug is a blocking function

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

// send buffer contents out via printf
// (filename, funcname, message)
//=============================================================================
            auto UsbDebug::
debug       (const char* fil, const char* fnam, const char* fmt, ...) -> void
            {
            if( not m_enable ) return;
            va_list args;
            va_start(args, fmt);

            printf( "[$1%010u$7]", Cp0::count() );
            printf( "[$2%-14s$7]", fil ); //filename
            printf( "[$5%-14s$7] ", fnam );
            vprintf(fmt, args); //variadic arguments, use vprintf
            printf("\r\n");

            va_end(args);
            }

