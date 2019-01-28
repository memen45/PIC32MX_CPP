#include "Putc.hpp"
#include "Uart.hpp"

//which uart to use
static Uart* m_uart;
//simple ansi enable
static bool m_ansi_on = true;
static bool m_ansi_triggered = false;
static const char m_ansi_trigchar = '@';
//color code start
static const char m_ansi_colorfg[] = "\033[38;5;";
static const char m_ansi_colorbg[] = "\033[48;5;";
//use 256 color palette
static const char* m_colors[] = {
    "000m", //K
    "196m", //R
    "10m",  //G
    "11m",  //Y
    "26m",  //B
    "135m", //M
    "39m",  //C
    "15m",  //W
};
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
                //if was not "$$", need to print previously suppressed '$'
                if( c != m_ansi_trigchar ) m_uart->putc(m_ansi_trigchar);
            }
            m_uart->putc(c);
            }

