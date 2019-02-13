#include "UsbDebug.hpp"
#include <cstdio>   //printf
#include "Cp0.hpp"  //print cp0 count
#include <cstdarg>  //variadic args


bool UsbDebug::m_enable{false};
Uart* UsbDebug::m_uart{nullptr};


// set uart device, optionally enable/disable (default = enable)
//=============================================================================
            auto UsbDebug::
debug       (Uart* u, bool tf) -> void
            {
            m_uart = u;
            m_enable = tf;
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

// printf type
//=============================================================================
            auto UsbDebug::
debug       (char const *fmt, ...) -> void
            {
            if(not m_uart or not m_enable) return;
            va_list args;
            va_start(args, fmt);
            m_uart->printf(fmt, args);
            va_end(args);
            }

// printf type with filename,function header
//=============================================================================
            auto UsbDebug::
debug       (char const* fil, char const* func, char const* fmt, ...) -> void
            {
            if(not m_uart or not m_enable) return;
            va_list args;
            va_start(args, fmt);
            m_uart->printf( "[@B%010u@W]", Cp0::count() );
            m_uart->printf( "[@M%-14s@W][@G%-14s@W] ", fil, func );
            m_uart->printf( fmt, args );
            va_end(args);
            m_uart->puts( "\r\n" );
            }
