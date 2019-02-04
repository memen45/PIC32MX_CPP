#pragma once

#include <cstdio>  //printf
#include <cstdbool>
#include "Uart.hpp"
#include "Cp0.hpp"

struct UsbDebug  {

    UsbDebug(){}        //empty constructor
    UsbDebug(Uart*);    //or specify a uart device

    //(filename, funcname, format, args...)
    static
    void    debug       (const char*, const char*, const char*, ... );

    static
    void    debug       (bool);     //enable/disable

    static
    bool    debug       (void);         //get enable/disable

    static
    void    debug       (Uart*);    //set which uart device

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
            m_uart->printf( "[@R%010u@W]", Cp0::count() );
            m_uart->printf( "[@M%-14s@W][@G%-14s@W] ", fil, func );
            m_uart->printf( fmt, args... );
            m_uart->puts( "\r\n" );
            }

    private:

            static Uart* m_uart;
            static bool m_enable;
};

