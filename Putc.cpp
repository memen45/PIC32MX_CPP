#include "Putc.hpp"
#include "Uart.hpp"

//which uart to use
static Uart* m_uart;
//simple ansi enable
static bool m_ansi_on = true;
static bool m_ansi_triggered = false;
static const char m_ansi_trigchar = '@';
//color code, replace index [3] with 0-7, index [2] with 3 (fg) or 4 (bg)
static char m_ansi_color[] = "\033[37m";
//reset ansi atrtibutes, colors
static const char m_ansi_reset[] = "\033[0;37;40m";


// set which uart to use
//=============================================================================
            auto Putc::
use         (Uart* u) -> void
            {
            if(u) m_uart = u;
            }

// enable/disable simple ansi markdown language
//=============================================================================
            auto Putc::
ansi        (bool tf) -> void
            {
            m_ansi_on = tf;
            m_ansi_triggered = false; //reset
            }

//=============================================================================
//printf putc replacement
//also have simple 'markdown' for ansi colors
//trigger char set above- assume '@' for these comments
//foreground color - K R G Y B M C W
//black red green yellow blue magenta cyan white
//backround color - k r g y b m c w
//reset ansi attributes and color - "@!"
//set foreground to white - "@W" (uppercase)
//set background to black - "@k" (lowercase)
//normal trigger char usage- @@ = '@', @@K = @K
//=============================================================================
            extern "C" auto
_mon_putc   (char c) -> void
            {
            if( not m_uart ) return;
            if( not m_ansi_on ){ m_uart->putc(c); return; }
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
                    m_ansi_color[2] = (n >> 3) + '3'; //'3' or '4'
                    m_ansi_color[3] = (n bitand 7) + '0'; //'0' - '7'
                    m_uart->puts(m_ansi_color);
                    return;
                }
                if( c == '!' ){
                    m_uart->puts(m_ansi_reset);
                    return; //done here
                }
                //if was not "$$", need to print previously suppressed '$'
                if( c != m_ansi_trigchar ) m_uart->putc(m_ansi_trigchar);
            }
            m_uart->putc(c);
            }

