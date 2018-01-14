#pragma once

#include <cstdint>
#include "Reg.hpp"


/*=============================================================================
 UART 1/2/3 functions
=============================================================================*/

struct Uart  {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate Uart with uart number
    enum UARTX { UART1 = 0, UART2, UART3 };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    constexpr Uart(UARTX);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxmode

    enum CLKSEL { PBCLK = 0, SYSCLK, FRC, REFO1 };
    enum RTSMODE { FLOW = 0, SIMPLEX };
    enum RXPOL { IDLEHIGH = 0, IDLELOW };
    enum MODESEL {
         MODE8N1 = 0, MODE8E1 = 2, MODE8O1 = 4, MODE9N1 = 6,
         MODE8N2 = 1, MODE8E2 = 3, MODE8O2 = 5, MODE9N2 = 7
    };

    void            stop_sleep      (bool);             //stop in sleep
    bool            active          ();                 //uart is active
    void            clk_sel         (CLKSEL);           //clock select
    void            oflow_stop      (bool);             //stop on OERR
    void            on              (bool);             //uart on/off
    void            stop_idle       (bool);             //uart stop in idle
    void            irda            (bool);             //irda on/off
    void            rts_mode        (RTSMODE);          //rts mode
    void            wake            (bool);             //wake on start bit
    void            loopback        (bool);             //loopback on/off
    void            autobaud        (bool);             //autobaud on/off
    void            rx_pol          (RXPOL);            //rx polarity
    void            hispeed         (bool);             //(hign)4x or 16x
    void            mode            (MODESEL);          //parity, data, stop

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxsta

    enum UTXISEL : uint8_t { TFREE = 0, TDONE, TEMPTY };
    enum URXISEL : uint8_t { RANY = 0, RHALF, RMOST };

    void            rx_mask         (uint8_t);          //address mask
    void            rx_addr         (uint8_t);          //address
    void            tx_irq          (UTXISEL);          //tx irq select
    void            tx_pol          (RXPOL);            //tx polarity
    void            rx_on           (bool);             //rx enable
    void            tx_break        ();                 //send break
    void            tx_on           (bool);             //tx enable
    bool            tx_full         ();                 //tx buf is full
    bool            tx_done         ();                 //all tx bits are out
    void            rx_irq          (URXISEL);          //rx irq select
    void            rx_addren       (bool);             //rx addr detect on
    bool            rx_busy         ();                 //rx busy (not idle)
    bool            rx_perr         ();                 //rx parity err
    bool            rx_ferr         ();                 //rx framing err
    bool            rx_oerr         ();                 //rx overrun err
    bool            rx_empty        ();                 //rx is empty

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxtxreg

    void            tx              (uint16_t);         //put in tx register

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxrxreg

    uint16_t        rx              ();                 //get from rx register

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxbrg

    void            baud_set        (uint32_t);         //set baud to value
    void            baud_set        ();                 //recalc, or default
    uint32_t        baud_clk        ();                 //get uart clock freq

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    Reg r;

    enum {
        U1MODE = 0xBF801800, UARTX_SPACING = 0x40,  //spacing in words
            SLPEN = 1<<23,
            ACTIVE = 1<<22,
            CLKSEL_SHIFT = 17, CLKSEL_CLR = 3,
            OVFDIS = 1<<16,
            ON = 1<<15,
            SIDL = 1<<13,
            IREN = 1<<12,
            RTSMD = 1<<11,
            WAKE = 1<<7,
            LPBACK = 1<<6,
            ABAUD = 1<<5,
            RXINV = 1<<4,
            BRGH = 1<<3,
            MODE_SHIFT = 0, MODE_CLR = 7,
        //U1STA
            MASK_SHIFT = 24, MASK_CLR = 255, //is byte3
            ADDR_SHIFT = 16, ADDR_CLR = 255, //is byte2
            UTXISEL_SHIFT = 14, UTXISEL_CLR = 3,
            UTXINV = 1<<13,
            URXEN = 1<<12,
            UTXBRK = 1<<11,
            UTXEN = 1<<10,
            UTXBF = 1<<9,
            TRMT = 1<<8,
            URXISEL_SHIFT = 6, URXISEL_CLR = 3,
            ADDEN = 1<<5,
            RIDLE = 1<<4,
            PERR = 1<<3,
            FERR = 1<<2,
            OERR = 1<<1,
            URXDA = 1<<0
    };

    volatile uint32_t* m_uartx_mode;
    volatile uint32_t* m_uartx_stat;
    volatile uint32_t& m_uartx_tx;                  //use reference
    volatile uint32_t& m_uartx_rx;                  //use reference
    volatile uint32_t& m_uartx_brg;                 //use reference

    uint32_t m_uartx_baud;                          //desired baud

    using vbyte_ptr = volatile uint8_t*;            //access stat as bytes

};

#include "Osc.hpp"
/*=============================================================================
 inline functions
=============================================================================*/
constexpr Uart::Uart(UARTX e)
    : m_uartx_mode((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)),
      m_uartx_stat((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+4),
      m_uartx_tx(*((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+8)),
      m_uartx_rx(*((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+12)),
      m_uartx_brg(*((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+16)),
      m_uartx_baud(0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxtxreg

void Uart::tx(uint16_t v){
    m_uartx_tx = v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxrxreg

uint16_t Uart::rx(){
    return m_uartx_rx;
}


