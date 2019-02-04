#include "Putc.hpp"
#include "Uart.hpp"
#include "UsbEP.hpp" //for TX

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
//or use usb cdc
static UsbCdcAcm* m_cdc;

// set which uart to use,  (Uart*)0 to disable output
//=============================================================================
            auto Putc::
use         (Uart* u) -> void
            {
            m_uart = u;
            m_cdc = 0;
            }

// set cdc to use,  (UsbCdcAcm*)0 to disable output
//=============================================================================
            auto Putc::
use         (UsbCdcAcm* u) -> void
            {
            m_cdc = u;
            m_uart = 0;
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
//private
//uart blocks on puts/putc- no worry
//usb cdc needs to block here on write to make sure value does not go
// out of scope before the usb gets around to sending, so wait for
// cdc tx available, send, wait for available again- value no longer needed
            static auto
do_puts     (const char* str) -> void
            {
            if(m_uart) m_uart->puts(str);
            else if(m_cdc){
                while(m_cdc->busy(UsbEP::TX));
                m_cdc->write(str);
                while(m_cdc->busy(UsbEP::TX));
            }
            }

            static auto
do_putc     (const char c) -> void
            {
            if(m_uart) m_uart->putc(c);
            else if(m_cdc){
                while(m_cdc->busy(UsbEP::TX));
                m_cdc->write((uint8_t*)&c, 1);
                while(m_cdc->busy(UsbEP::TX));
            }
            }

            extern "C" auto
_mon_putc   (char c) -> void
            {
            if( not m_uart and not m_cdc ) return;
            if( c == m_ansi_trigchar and not m_ansi_triggered ){
                m_ansi_triggered = true;
                return;
            }

            //ansi is on, and c was not a new trigger

            if( m_ansi_triggered ){
                static const char markdown[] = "KRGYBMCWkrgybmcw";
                m_ansi_triggered = false;
                uint8_t n = 0;
                for( ; markdown[n] and (markdown[n] != c); n++ );
                if( markdown[n] ){ //found
                    if( not m_ansi_on ) return;
                    if( n bitand 8 ){
                        return do_puts(m_ansi_colorbg),
                             do_puts(m_colors[n bitand 7]);
                    } else {
                        return do_puts(m_ansi_colorfg),
                            do_puts(m_colors[n bitand 7]);
                    }
                }
                if( c == '!' ) return do_puts(m_ansi_reset);
                if( c == '-' || c == '+' ){
                    m_ansi_on = c == '-' ? false : true;
                    return;
                }
                //if was not "@@", need to print previously suppressed '@'
                if( c != m_ansi_trigchar ) do_putc(m_ansi_trigchar);
            }
            do_putc(c);
            }

