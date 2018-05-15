#pragma once

#include <cstdint>

#include "Pins.hpp"

//UART 1/2/3

struct Uart  {

    //instantiate Uart with uart number only (pins setup manually)-
    //(use this for UART1- has fixed pins, so cannot use pps)
    //  Uart u(UART1);

    //or uart number and tx/rx pin info (tx or rx only), optionally baud rate-
    //  UARTnTX|RX, tx|rx pin, [baud]
    //  Uart u(UART2TX, Pins::A0);          //115200 default baud
    //  Uart u(UART2TX, Pins::A0, 19200);   //specify baud
    //  Uart u(UART2RX, Pins::A0, 19200);   //specify baud

    //or uart number and both tx/rx pins info, optionally set baud rate
    //  UARTn, tx pin, rx pin,  [baud]
    //  Uart u(UART2, Pins::A0, Pins::B0);  //uart#, tx pin, rx pin, [baud]
    //  Uart u(UART2, Pins::A0, Pins::B0, 230400);

    enum UARTX {
        UART1 = 0, UART1TX = UART1|1<<2, UART1RX = UART1,
        UART2 = 1, UART2TX = UART2|1<<2, UART2RX = UART2,
        UART3 = 2, UART3TX = UART3|1<<2, UART3RX = UART3
    };
    Uart(UARTX);
    Uart(UARTX, Pins::RPN, uint32_t = 0); //tx/rx only, bit2=1=tx bit2=0=rx
    Uart(UARTX, Pins::RPN, Pins::RPN, uint32_t = 0);

    //uxmode

    auto
    stop_sleep (bool) -> void;          //stop in sleep

    auto
    active () -> bool;                  //uart is active

    enum CLKSEL { PBCLK, SYSCLK, FRC, REFO1 };
    auto
    clk_sel (CLKSEL) -> void;           //clock select

    auto
    oflow_stop (bool) -> void;          //stop on OERR

    auto
    on (bool) -> void;                  //uart on/off

    auto
    stop_idle (bool) -> void;           //uart stop in idle

    auto
    irda (bool) -> void;                //irda on/off

    enum RTSMODE { FLOW, SIMPLEX };
    auto
    rts_mode (RTSMODE) -> void;         //rts mode

    auto
    wake (bool) -> void;                //wake on start bit

    auto
    loopback (bool) -> void;            //loopback on/off

    auto
    autobaud (bool) -> void;            //autobaud on/off

    enum RXPOL { IDLEHIGH, IDLELOW };
    auto
    rx_pol (RXPOL) -> void;             //rx polarity

    enum MODESEL {
         MODE8N1 = 0, MODE8E1 = 2, MODE8O1 = 4, MODE9N1 = 6,
         MODE8N2 = 1, MODE8E2 = 3, MODE8O2 = 5, MODE9N2 = 7
    };
    auto
    mode (MODESEL) -> void;             //parity, data, stop

    //uxsta

    auto
    rx_mask (uint8_t) -> void;          //address mask

    auto
    rx_addr (uint8_t) -> void;          //address

    enum UTXISEL : uint8_t { TFREE, TDONE, TEMPTY };
    auto
    tx_irq (UTXISEL) -> void;           //tx irq select

    auto
    tx_pol (RXPOL) -> void;             //tx polarity

    auto
    rx_on (bool) -> void;               //rx enable

    auto
    tx_break () -> void;                //send break

    auto
    tx_on (bool) -> void;               //tx enable

    auto
    tx_full () -> bool;                 //tx buf is full

    auto
    tx_done () -> bool;                 //all tx bits are out

    enum URXISEL : uint8_t { RANY, RHALF, RMOST };
    auto
    rx_irq (URXISEL) -> void;           //rx irq select

    auto
    rx_addren (bool) -> void;           //rx addr detect on

    auto
    rx_busy () -> bool;                 //rx busy (not idle)

    auto
    rx_perr () -> bool;                 //rx parity err

    auto
    rx_ferr () -> bool;                 //rx framing err

    auto
    rx_oerr () -> bool;                 //rx overrun err

    auto
    rx_empty () -> bool;                //rx is empty

    //uxtxreg

    auto
    write (uint16_t) -> void;           //put in tx register

    //uxrxreg

    auto
    read () -> uint16_t;                //get from rx register

    //uxbrg

    auto
    baud_set (uint32_t) -> void;        //set baud to value

    auto
    baud_set () -> void;                //recalc, or default

    auto
    baud_clk () -> uint32_t;            //get uart clock freq

    //misc

    auto
    putchar (const char) -> void;       //blocking functions

    auto
    puts (const char*) -> void;         //(no '\n', like fputs)

    auto
    getchar () -> int;                  // -1 = none


    private:

    //baud_set will take care of- if normal speed error >2%, then hispeed
    auto
    hispeed (bool) -> void;             //(true)4x or 16x


    volatile uint32_t* m_uartx_base;
    volatile uint32_t& m_uartx_tx;                  //use reference
    volatile uint32_t& m_uartx_rx;                  //use reference
    uint32_t m_uartx_baud;                          //desired baud

};
