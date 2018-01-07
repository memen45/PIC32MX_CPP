#pragma once

#include <cstdint>
#include "Reg.hpp"


/*=============================================================================
 UART 1/2/3 functions
=============================================================================*/

struct Uart123  {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate Uart123 with uart number
    enum UARTX { UART1 = 0, UART2, UART3 };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    constexpr Uart123(UARTX);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxmode

    enum {
        SLPEN = 1<<23,
        ACTIVE = 1<<22,
        CLKSEL_SHIFT = 17, CLKSEL_CLR = 3,
        OVFDIS = 1<<16,
        ON = 1<<15,
        SIDL = 1<<13,
        IREN = 1<<12,
        RTSMD = 1<<11,      SIMPLEX = 1,    FLOW = 0,
        WAKE = 1<<7,
        LPBACK = 1<<6,
        ABAUD = 1<<5,
        RXINV = 1<<4,       IDLELOW = 1,    IDLEHIGH = 0,
        BRGH = 1<<3,        BRG4x = 1,      BRG16x = 0,
        MODE_CLR = 7
    };

    enum CLKSEL { PBCLK = 0, SYSCLK, FRC, REFO1 };
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
    void            rts_mode        (bool);             //rts mode
    void            wake            (bool);             //wake on start bit
    void            loopback        (bool);             //loopback on/off
    void            autobaud        (bool);             //autobaud on/off
    void            rx_pol          (bool);             //rx polarity
    void            brg_mode        (bool);             //4x or 16x
    void            mode            (MODESEL);          //parity, data, stop

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxsta


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxtxreg

    void            tx              (uint16_t);         //put in tx register

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxrxreg

    uint16_t        rx              ();                 //get from rx register

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //uxbrg

    void            set_baud        (uint32_t);
    void            update_brg      ();
    uint32_t        clk_freq        ();                 //get uart clock freq


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    Reg r;

    enum {
        U1MODE = 0xBF801800, UARTX_SPACING = 0x40,  //spacing in words
    };

    volatile uint32_t* m_uartxmode;
    volatile uint32_t* m_uartxstat;
    volatile uint32_t& m_uartxtx;                   //use reference
    volatile uint32_t& m_uartxrx;                   //use reference
    volatile uint32_t& m_uartxbrg;                  //use reference

    uint32_t m_uartx_baud;                          //desired baud
};

#include "Osc.hpp"
/*=============================================================================
 inline functions
=============================================================================*/
constexpr Uart123::Uart123(UARTX e)
    : m_uartxmode((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)),
      m_uartxstat((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+4),
      m_uartxtx(*((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+8)),
      m_uartxrx(*((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+12)),
      m_uartxbrg(*((volatile uint32_t*)U1MODE+(e*UARTX_SPACING)+16)),
      m_uartx_baud(0)
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxtxreg

void Uart123::tx(uint16_t v){
    m_uartxtx = v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//uxrxreg

uint16_t Uart123::rx(){
    return m_uartxrx;
}


