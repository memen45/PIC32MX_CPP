#include "Putc.hpp"
#include "Uart.hpp"

//which uart to use
static Uart* m_uart;

// set which uart to use
//=============================================================================
            auto Putc::
set         (Uart* u) -> void
            {
            if(u) m_uart = u;
            }

//=============================================================================
//printf putc replacement
//also have simple 'markdown' for ansi colors
// $0 = black, $7 = white, $$ = '$'
//=============================================================================
            extern "C" auto
_mon_putc   (char c) -> void
            {
            static bool trigger = false;
            if( not m_uart ) return;
            if(trigger){
                trigger = false;
                if(c >= '0' && c <= '7'){
                    m_uart->puts("\033[3"); //ansi color start
                    m_uart->putc(c);        //plus color
                    c = 'm';                //below will output this
                }
                m_uart->putc(c);            //'m' from above, or regular char after '$'
                return;
            }
            //not triggered
            if(c == '$') trigger = true;    //trigger char
            else m_uart->putc(c);           //regular char
            }

