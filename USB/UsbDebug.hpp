#pragma once

#include <cstdio>  //printf
#include <cstdbool>
#include "Uart.hpp"
#include "Cp0.hpp"

struct UsbDebug  {

            //empty constructor
UsbDebug    ()
            {
            }

            //or specify a uart device
UsbDebug    (Uart*);

            //(filename, funcname, format, args...)
            static auto
debug       (const char*, const char*, const char*, ... ) -> void;

            //enable/disable
            static auto
debug       (bool) -> void;

            //get enable/disable
            static auto
debug       (void) -> bool;

            //set which uart device
            static auto
debug       (Uart*) -> void;

            //printf type
            template<typename... Args>
            auto
debug       (char const *fmt, Args... args) -> void
            {
            if(m_uart) m_uart->printf(fmt, args...);
            }

            //printf type with filename,function header
            template<typename... Args>
            auto
debug       (char const* fil, char const* func, char const* fmt, Args... args) -> void
            {
            if(not m_uart) return;
            m_uart->printf( "[@B%010u@W]", Cp0::count() );
            m_uart->printf( "[@M%-14s@W][@G%-14s@W] ", fil, func );
            m_uart->printf( fmt, args... );
            m_uart->puts( "\r\n" );
            }

    private:

            static Uart* m_uart;
            static bool m_enable;
};

