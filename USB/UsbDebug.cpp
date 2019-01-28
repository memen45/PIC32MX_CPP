#include "UsbDebug.hpp"
#include <cstdio>   //printf
#include "Cp0.hpp"  //print cp0 count
#include <cstdarg>  //variadic args
#include "Uart.hpp" //any uart device can de used for output
#include "Putc.hpp"

//debug is a blocking function

static bool m_enable{true};

// constructor specifying uart
//=============================================================================
            UsbDebug::
UsbDebug    (Uart* u)
            {
            Putc::use(u);
            }

// set putc device
//=============================================================================
            auto UsbDebug::
debug       (Uart* u) -> void
            {
            Putc::use(u);
            }

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

            printf( "[@R%010u@W]", Cp0::count() );
            printf( "[@M%-14s@W]", fil ); //filename
            printf( "[@G%-14s@W] ", fnam );
            vprintf(fmt, args); //variadic arguments, use vprintf
            printf("\r\n");

            va_end(args);
            }

