#pragma once

#include <cstdint>

#include "Pins.hpp"
#include "Osc.hpp"

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

    enum UARTX { UART1 = 0, UART4 = 1, UART3 = 2,
                    UART6 = 3, UART2 = 4, UART5 = 5,
    };
    Uart(UARTX, uint32_t = 0);

    //uxmode
    void            on              (bool);             //uart on/off
    void            stop_idle       (bool);             //uart stop in idle
    void            irda            (bool);             //irda on/off

    enum RTSMODE { FLOW, SIMPLEX };
    void            rts_mode        (RTSMODE);          //rts mode

    enum UEN { BCLK, CTS_RTS, RTS, TX_RX };
    void            uen             (UEN);              //enable uart pins
    void            wake            (bool);             //wake on start bit
    void            loopback        (bool);             //loopback on/off
    void            autobaud        (bool);             //autobaud on/off

    enum RXPOL { IDLEHIGH, IDLELOW };
    void            rx_pol          (RXPOL);            //rx polarity

    enum MODESEL {
         MODE8N1 = 0, MODE8E1 = 2, MODE8O1 = 4, MODE9N1 = 6,
         MODE8N2 = 1, MODE8E2 = 3, MODE8O2 = 5, MODE9N2 = 7
    };
    void            mode            (MODESEL);          //parity, data, stop

    //uxsta
    void            rx_mask         (bool);             //address mask
    void            rx_addr         (uint8_t);          //address

    enum UTXISEL : uint8_t { TFREE, TDONE, TEMPTY };
    void            tx_irq          (UTXISEL);          //tx irq select

    //enum RXPOL { IDLEHIGH = 0, IDLELOW }; //same
    void            tx_pol          (RXPOL);            //tx polarity
    void            rx_on           (bool);             //rx enable
    void            tx_break        ();                 //send break
    void            tx_on           (bool);             //tx enable
    bool            tx_full         ();                 //tx buf is full
    bool            tx_done         ();                 //all tx bits are out

    enum URXISEL : uint8_t { RANY, RHALF, RMOST };
    void            rx_irq          (URXISEL);          //rx irq select

    void            rx_addren       (bool);             //rx addr detect on
    bool            rx_busy         ();                 //rx busy (not idle)
    bool            rx_perr         ();                 //rx parity err
    bool            rx_ferr         ();                 //rx framing err
    bool            rx_oerr         ();                 //rx overrun err
    bool            rx_empty        ();                 //rx is empty

    //uxtxreg
    void            write           (uint16_t);         //put in tx register

    //uxrxreg
    uint16_t        read            ();                 //get from rx register

    //uxbrg
    void            baud_set        (uint32_t);         //set baud to value
    void            baud_set        ();                 //recalc, or default
    uint32_t        baud_clk        ();                 //get uart clock freq

    //misc
    void            putchar         (const char);       //blocking functions
    void            puts            (const char*);      //(no '\n', like fputs)
    int             getchar         ();                 // -1 = none

    private:

    //baud_set will take care of- if normal speed error >2%, then hispeed
    void            hispeed         (bool);             //(true)4x or 16x

    volatile uint32_t* m_uartx_base;
    volatile uint32_t& m_uartx_tx;                  //use reference
    volatile uint32_t& m_uartx_rx;                  //use reference

    uint32_t m_uartx_baud;                          //desired baud

};

//=============================================================================
inline   uint32_t    Uart::baud_clk          ()
//=============================================================================
{
    return Osc::pbclk(); //pb/sys are the same
}
