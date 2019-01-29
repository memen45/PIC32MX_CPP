#include "Putc.hpp"
#include "Uart.hpp"


//simple ansi
static bool m_ansi_on = true;
static bool m_ansi_triggered = false;
static const char m_ansi_trigchar = '@';
static const char m_ansi_colorfg[] = "\033[38;5;";
static const char m_ansi_colorbg[] = "\033[48;5;";
//use 256 color palette for a little more control
//appended to _fg[] _bg above
static const char* m_colors[] = {
    "0m",   //K
    "196m", //R
    "48m",  //G
    "226m", //Y
    "26m",  //B
    "163m", //M
    "6m",  //C
    "15m",  //W
};
//reset ansi atrtibutes, colors
static const char m_ansi_reset[] = "\033[0;38;5;15m\033[48;5;0m";

//which uart to use
static Uart* m_uart;

// set which uart to use,  (Uart*)0 to disable output
//=============================================================================
            auto Putc::
use         (Uart* u) -> void
            {
            if(u) m_uart = u;
            }

//=============================================================================
//printf putc replacement
//also have simple 'markdown' for ansi colors
//trigger char set above- assume '@' for these comments
//foreground color - K R G Y B M C W
//black red green yellow blue magenta cyan white
//backround color - k r g y b m c w
//reset ansi attributes and color - "@!"
//turn off/on ansi color "@-" "@+" (still have to use "@@" for "@")
//set foreground to white - "@W" (uppercase)
//set background to black - "@k" (lowercase)
//normal trigger char usage- @@ = '@', @@K = @K
//=============================================================================
            extern "C" auto
_mon_putc   (char c) -> void
            {
            if( not m_uart ) return;
            if( c == m_ansi_trigchar and not m_ansi_triggered ){
                m_ansi_triggered = true;
                return;
            }

            //ansi is on, and c was not a new trigger

            if( m_ansi_triggered ){
                static const char markdown[] = "KRGYBMCWkrgybmcw";
                m_ansi_triggered = false;
                uint8_t n = 0;
                for( ; markdown[n]; n++ ){
                    if( markdown[n] == c ) break;
                }
                if( markdown[n] ){ //found
                    if( not m_ansi_on ) return;
                    if( n bitand 8 ){
                        m_uart->puts(m_ansi_colorbg);
                        m_uart->puts(m_colors[n bitand 7]);
                    } else {
                        m_uart->puts(m_ansi_colorfg);
                        m_uart->puts(m_colors[n bitand 7]);
                    }
                    return;
                }
                if( c == '!' ){
                    m_uart->puts(m_ansi_reset);
                    return; //done here
                }
                if( c == '-' || c == '+' ){
                    m_ansi_on = c == '-' ? false : true;
                    return; //done here
                }
                //if was not "@@", need to print previously suppressed '@'
                if( c != m_ansi_trigchar ) m_uart->putc(m_ansi_trigchar);
            }
            m_uart->putc(c);
            }

