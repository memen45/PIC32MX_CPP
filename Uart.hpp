#pragma once

#include <cstdint>

#include "Pins.hpp"

//UART 1/2/3

struct Uart  {

    //instantiate Uart with uart number
    enum UARTX { UART1 = 0, UART2, UART3 };
    Uart(UARTX);
    Uart(UARTX, Pins::RPN, Pins::RPN, uint32_t = 0);

    //uxmode
    void            stop_sleep      (bool);             //stop in sleep
    bool            active          ();                 //uart is active

    enum CLKSEL { PBCLK = 0, SYSCLK, FRC, REFO1 };
    void            clk_sel         (CLKSEL);           //clock select

    void            oflow_stop      (bool);             //stop on OERR
    void            on              (bool);             //uart on/off
    void            stop_idle       (bool);             //uart stop in idle
    void            irda            (bool);             //irda on/off

    enum RTSMODE { FLOW = 0, SIMPLEX };
    void            rts_mode        (RTSMODE);          //rts mode

    void            wake            (bool);             //wake on start bit
    void            loopback        (bool);             //loopback on/off
    void            autobaud        (bool);             //autobaud on/off

    enum RXPOL { IDLEHIGH = 0, IDLELOW };
    void            rx_pol          (RXPOL);            //rx polarity

    void            hispeed         (bool);             //(high)4x or 16x

    enum MODESEL {
         MODE8N1 = 0, MODE8E1 = 2, MODE8O1 = 4, MODE9N1 = 6,
         MODE8N2 = 1, MODE8E2 = 3, MODE8O2 = 5, MODE9N2 = 7
    };
    void            mode            (MODESEL);          //parity, data, stop

    //uxsta
    void            rx_mask         (uint8_t);          //address mask
    void            rx_addr         (uint8_t);          //address

    enum UTXISEL : uint8_t { TFREE = 0, TDONE, TEMPTY };
    void            tx_irq          (UTXISEL);          //tx irq select

    //enum RXPOL { IDLEHIGH = 0, IDLELOW }; //same
    void            tx_pol          (RXPOL);            //tx polarity
    void            rx_on           (bool);             //rx enable
    void            tx_break        ();                 //send break
    void            tx_on           (bool);             //tx enable
    bool            tx_full         ();                 //tx buf is full
    bool            tx_done         ();                 //all tx bits are out

    enum URXISEL : uint8_t { RANY = 0, RHALF, RMOST };
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
    void            putc            (const char);       //simple blocking
    void            puts            (const char*);      //functions
    int             getc            ();                 // -1 = none

    private:

    volatile uint32_t* m_uartx_base;
    volatile uint32_t& m_uartx_tx;                  //use reference
    volatile uint32_t& m_uartx_rx;                  //use reference

    uint32_t m_uartx_baud;                          //desired baud

};
