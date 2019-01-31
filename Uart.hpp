#pragma once

#include <cstdint>

#include "Pins.hpp"
#include "Osc.hpp"


//UART 1/2/3

struct Uart {

    //instantiate Uart with uart number, optionally baud rate-
    //(pins setup manually)
    //(use this for UART1- has fixed pins, so cannot use pps)
    //  UARTn, [, baud ]
    //  Uart u(UART1);              //tx+rx, default baud 115200
    //  Uart u(UART1TX, 230400);    //tx only, 230400 baud

    //or uart number and tx/rx pin info (tx or rx only), optionally baud rate-
    //  UARTnTX|RX, tx|rx pin [, baud ]
    //  Uart u(UART2TX, Pins::A0);          //tx, 115200 default baud
    //  Uart u(UART2TX, Pins::A0, 19200);   //tx, 19200 baud
    //  Uart u(UART2RX, Pins::A0, 230400);  //rx, 230400 baud

    //or uart number and both tx/rx pins info, optionally set baud rate
    //  UARTn, tx pin, rx pin [, baud ]
    //  Uart u(UART2, Pins::A0, Pins::B0);
    //  Uart u(UART2, Pins::A0, Pins::B0, 230400);

            enum
UARTX       {
            UART1 = 0, UART4 = 1, UART3 = 2,
            UART6 = 3, UART2 = 4, UART5 = 5,
            };

Uart        (UARTX, uint32_t = 0);


            //==== UxMODE ====

            //uart on/off
            auto
on          (bool) -> void;

            //uart stop in idle
            auto
stop_idle   (bool) -> void;

            //irda on/off
            auto
irda        (bool) -> void;

            enum
RTSMODE     { FLOW, SIMPLEX };

            //rts mode
            auto
rts_mode    (RTSMODE) -> void;

			enum
UEN			{ 
			BCLK, CTS_RTS, RTS, TX_RX 
			};
			
			//enable uart pins
			auto
uen			(UEN) -> void;

            //wake on start bit
            auto
wake        (bool) -> void;

            //loopback on/off
            auto
loopback    (bool) -> void;

            //autobaud on/off
            auto
autobaud    (bool) -> void;

            enum
RXPOL       { IDLEHIGH, IDLELOW };

            //rx polarity
            auto
rx_pol      (RXPOL) -> void;

            enum
MODESEL     {
            MODE8N1 = 0, MODE8E1 = 2, MODE8O1 = 4, MODE9N1 = 6,
            MODE8N2 = 1, MODE8E2 = 3, MODE8O2 = 5, MODE9N2 = 7
            };

            //parity, data, stop
            auto
mode        (MODESEL) -> void;


            //==== UxSTA ====

            //address mask
            auto
rx_mask     (bool) -> void;

            //address
            auto
rx_addr     (uint8_t) -> void;

            enum
UTXISEL     : uint8_t { TFREE, TDONE, TEMPTY };

            //tx irq select
            auto
tx_irq      (UTXISEL) -> void;

            //tx polarity
            auto
tx_pol      (RXPOL) -> void;

            //rx enable
            auto
rx_on       (bool) -> void;

            //send break
            auto
tx_break    () -> void;

            //tx enable
            auto
tx_on       (bool) -> void;

            //tx buf is full?
            auto
tx_full     () -> bool;

            //all tx bits are out
            auto
tx_done     () -> bool;

            enum
URXISEL     : uint8_t { RANY, RHALF, RMOST };

            //rx irq select
            auto
rx_irq      (URXISEL) -> void;

            //rx addr detect on
            auto
rx_addren   (bool) -> void;

            //rx busy? (not idle)
            auto
rx_busy     () -> bool;

            //rx parity err?
            auto
rx_perr     () -> bool;

            //rx framing err?
            auto
rx_ferr     () -> bool;

            //rx overrun err?
            auto
rx_oerr     () -> bool;

            //clear rx overrun err
            auto
rx_oerrclr  () -> void;

            //rx is empty?
            auto
rx_empty    () -> bool;


            //==== UxTXREG ====

            //put in tx register
            auto
write       (uint16_t) -> void;


            //==== UxRXREG ====

            //get from rx register
            auto
read        () -> uint16_t;


            //==== UxBRG ====

            //set baud to value
            auto
baud_set    (uint32_t) -> void;

            //recalc baud, or set to default
            auto
baud_set    () -> void;

            //get uart clock freq
            auto
baud_clk    () -> uint32_t;


            //misc
            //blocking functions

            auto
putc        (const char) -> void;

            //(no '\n', like fputs)
            auto
puts        (const char*) -> void;

            // -1 = none
            auto
getchar     () -> int;


            private:

            //baud_set will take care of-
            //if normal speed error >2%, then hispeed
            //true=4x=hispeed false=16x
            auto
hispeed     (bool) -> void;


            volatile uint32_t*  m_uartx_base;
            volatile uint32_t&  m_uartx_tx;     //use reference
            volatile uint32_t&  m_uartx_rx;     //use reference
            uint32_t            m_uartx_baud;   //desired baud
};

//=============================================================================
			inline auto Uart::
baud_clk	() -> uint32_t
			{
			return Osc::pbclk(); //pb/sys are the same
			}
