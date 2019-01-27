#include "UsbDebug.hpp"
#include <cstdio>  //printf
#include "Cp0.hpp"
#include <cstdarg> //variadic args
#include "Uart.hpp"

//_mon_putc has to be used so printf can go out our uart
// will also make printf universally available if printf
// used elsewhere

//debug is a blocking function

static bool m_enable{true};
static Uart* m_uart;

//printf putc replacement
//also have simple 'markdown' for ansi colors
// $0 = black, $7 = white, $$ = '$'
extern "C" void _mon_putc(char c){
    static bool trigger = false;
    if( not m_uart ) return;
    if(trigger){
        trigger = false;
        if(c >= '0' && c <= '7'){
            m_uart->puts("\033[3");    //ansi color start
            m_uart->putchar(c);        //plus color
            c = 'm';                //below will output this
        }
        m_uart->putchar(c);            //'m' from above, or regular char after '$'
        return;
    }
    //not triggered
    if(c == '$') trigger = true;    //trigger char
    else m_uart->putchar(c);           //regular char
}

// constructor specifying uart
//=============================================================================
            UsbDebug::
UsbDebug    (Uart*u)
            {
            m_uart = u;
            }

// enable/disable
//=============================================================================
            auto UsbDebug::
debug       (Uart* u) -> void
            {
            m_uart = u;
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
            if( not m_enable || not m_uart ) return;
            va_list args;
            va_start(args, fmt);

            printf( "[$1%010u$7]", Cp0::count() );
            printf( "[$2%-14s$7]", fil ); //filename
            printf( "[$5%-14s$7] ", fnam );
            vprintf(fmt, args); //variadic arguments, use vprintf
            printf("\r\n");

            va_end(args);
            }

