#include "Uart.hpp"
#include "Osc.hpp"
#include "Pins.hpp"


enum {
UARTX_SPACING = 0x40,  //spacing in words
U1MODE = 0xBF801800,
    SLPEN = 23,
    ACTIVE = 22,
    CLKSEL_SHIFT = 17, CLKSEL_MASK = 3,
    OVFDIS = 16,
    ON = 15,
    SIDL = 13,
    IREN = 12,
    RTSMD = 11,
    WAKE = 7,
    LPBACK = 6,
    ABAUD = 5,
    RXINV = 4,
    BRGH = 3,
    MODE_SHIFT = 0, MODE_MASK = 7,
UXSTA = 4, //offset from UXMODE in words
    MASK_SHIFT = 24, MASK_MASK = 255, //is byte3
    ADDR_SHIFT = 16, ADDR_MASK = 255, //is byte2
    UTXISEL_SHIFT = 14, UTXISEL_MASK = 3,
    UTXINV = 13,
    URXEN = 12,
    UTXBRK = 11,
    UTXEN = 10,
    UTXBF = 9,
    TRMT = 8,
    URXISEL_SHIFT = 6, URXISEL_MASK = 3,
    ADDEN = 5,
    RIDLE = 4,
    PERR = 3,
    FERR = 2,
    OERR = 1,
    URXDA = 0,
UXTXREG = 8,
UXRXREG = 12,
UXBRG = 16
            };

//Uart

//base uart instantiation
//UARTX-> <b3:0> -> |rx|tx|uartN| -> rx=b3 tx=b2 uartn=<b1:0>
//=============================================================================
            Uart::
Uart        (UARTX e, uint32_t baud)
            :
            m_uartx_base((vu32ptr)U1MODE + (((e bitand 3)-1) * UARTX_SPACING)),
            m_uartx_tx(*((vu32ptr)U1MODE + (((e bitand 3)-1) *
                UARTX_SPACING) + UXTXREG)),
            m_uartx_rx(*((vu32ptr)U1MODE + (((e bitand 3)-1) *
                UARTX_SPACING) + UXRXREG)),
            m_uartx_baud(baud)
            {
                if(e bitand (1<<2)) tx_on(true);
                if(e bitand (1<<3)) rx_on(true);
            }

//specify tx/rx pins (UART2,UART3)
//(there is no 'Pins' value of 0, so can use 0 to check if tx/rx pin not used)
//(when called by the following instantiation function with tx/rx only)
//=============================================================================
            Uart::
Uart        (UARTX e, Pins::RPN tx, Pins::RPN rx, uint32_t baud)
            :
            Uart(e, baud)
            {
            uint32_t u = (uint32_t)(e bitand 3); //uart 1-3
            //UART1 is fixed pins, no pps
            if(u == 1) return;
            //must be uart2 or uart3
            if(rx) {
                Pins r(rx, Pins::INPU);
                r.pps_in(u == 2 ? r.U2RX : r.U3RX);
            }
            if(tx){
                Pins t(tx, Pins::OUT);
                t.pps_out(u == 2 ? t.U2TX : t.U3TX);
            }
            }

//specify tx or rx pin only (UART2,UART3)
//use UARTnTX or UARTnRX
//=============================================================================
            Uart::
Uart        (UARTX e, Pins::RPN trx, uint32_t baud)
            :
            //overide any attempt to use both tx/rx when using UARTn
            //instead of correctly using UARTnTX or UARTnRX (rx overrides)
            Uart(e,
                (e bitand (1<<3)) ? (Pins::RPN)0 : trx, //tx
                (e bitand (1<<3)) ? trx : (Pins::RPN)0, //rx
                baud)
            {
            }

//uxtxreg
//=============================================================================
            auto Uart::
write       (uint16_t v) -> void
            {
            m_uartx_tx = v;
            }

//uxrxreg
//=============================================================================
            auto Uart::
read        () -> uint16_t
            {
            return m_uartx_rx;
            }


//uxmode
//=============================================================================
            auto Uart::
stop_sleep  (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<SLPEN, not tf);
            }

//=============================================================================
            auto Uart::
active      () -> bool
            {
            return anybit(m_uartx_base, 1<<ACTIVE);
            }

//=============================================================================
            auto Uart::
clk_sel     (CLKSEL e) -> void
            {
            clrbit(m_uartx_base, CLKSEL_MASK<<CLKSEL_SHIFT);
            setbit(m_uartx_base, e<<CLKSEL_SHIFT);
            baud_set();
            }

//=============================================================================
            auto Uart::
oflow_stop  (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<OVFDIS, not tf);
            }

//=============================================================================
            auto Uart::
on          (bool tf) -> void
            {
            baud_set(); //in case not set
            setbit(m_uartx_base, 1<<ON, tf);
            }

//=============================================================================
            auto Uart::
stop_idle   (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<SIDL, tf);
            }

//=============================================================================
            auto Uart::
irda        (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<IREN, tf);
            }

//=============================================================================
            auto Uart::
rts_mode    (RTSMODE e) -> void
            {
            setbit(m_uartx_base, 1<<RTSMD, e);
            }

//=============================================================================
            auto Uart::
wake        (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<WAKE, tf);
            }

//=============================================================================
            auto Uart::
loopback    (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<LPBACK, tf);
            }

//=============================================================================
            auto Uart::
autobaud    (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<ABAUD, tf);
            }

//=============================================================================
            auto Uart::
rx_pol      (RXPOL e) -> void
            {
            setbit(m_uartx_base, 1<<RXINV, e);
            }

//=============================================================================
            auto Uart::
hispeed     (bool tf) -> void
            {
            setbit(m_uartx_base, 1<<BRGH, tf);
            }

//=============================================================================
            auto Uart::
mode        (MODESEL e) -> void
            {
            clrbit(m_uartx_base, MODE_MASK<<MODE_SHIFT);
            setbit(m_uartx_base, e<<MODE_SHIFT);
            }

//uxsta
//=============================================================================
            auto Uart::
rx_mask     (uint8_t v) -> void
            {
            val((vu8ptr)m_uartx_base + (UXSTA * 4) + 3, v);
            }

//=============================================================================
            auto Uart::
rx_addr     (uint8_t v) -> void
            {
            val((vu8ptr)m_uartx_base + (UXSTA * 4) + 2, v);
            }

//=============================================================================
            auto Uart::
tx_irq      (UTXISEL e) -> void
            {
            clrbit(m_uartx_base + UXSTA, UTXISEL_MASK<<UTXISEL_SHIFT);
            setbit(m_uartx_base + UXSTA, e<<UTXISEL_SHIFT);
            }

//=============================================================================
            auto Uart::
tx_pol      (RXPOL e) -> void
            {
            bool b = anybit(m_uartx_base, 1<<IREN) ? not e : e;
            setbit(m_uartx_base + UXSTA, 1<<UTXINV, b);
            }

//=============================================================================
            auto Uart::
rx_on       (bool tf) -> void
            {
            setbit(m_uartx_base + UXSTA, 1<<URXEN, tf);
            }

//=============================================================================
            auto Uart::
tx_break    () -> void
            {
            setbit(m_uartx_base + UXSTA, 1<<UTXBRK);
            }

//=============================================================================
            auto Uart::
tx_on       (bool tf) -> void
            {
            setbit(m_uartx_base + UXSTA, 1<<UTXEN, tf);
            }

//=============================================================================
            auto Uart::
tx_full     () -> bool
            {
            return anybit(m_uartx_base + UXSTA, 1<<UTXBF);
            }

//=============================================================================
            auto Uart::
tx_done     () -> bool
            {
            return anybit(m_uartx_base + UXSTA, 1<<TRMT);
            }

//=============================================================================
            auto Uart::
rx_irq      (URXISEL e) -> void
            {
            clrbit(m_uartx_base + UXSTA, URXISEL_MASK<<URXISEL_SHIFT);
            setbit(m_uartx_base + UXSTA, e<<URXISEL_SHIFT);
            }

//=============================================================================
            auto Uart::
rx_addren   (bool tf) -> void
            {
            setbit(m_uartx_base + UXSTA, 1<<ADDEN, tf);
            }

//=============================================================================
            auto Uart::
rx_busy     () -> bool
            {
            return not anybit(m_uartx_base + UXSTA, 1<<RIDLE);
            }

//=============================================================================
            auto Uart::
rx_err      () -> uint8_t
            {
            return val8(m_uartx_base + UXSTA) bitand (1<<PERR|1<<FERR|1<<OERR);
            }

//clear rx overrun err (resets fifo by setting oerr, then clearing)
//if was framing/parity error- need to set the oerr first
//this way all errors can be cleared with this one function, even though
//framing/parity errors don't require such a drastic solution
//(at that point, things are already wrong, so the protocol above this will
// have to deal with errors anyway)
//=============================================================================
            auto Uart::
rx_errclr   () -> void
            {
            setbit(m_uartx_base + UXSTA, 1<<OERR);
            clrbit(m_uartx_base + UXSTA, 1<<OERR);
            }

//=============================================================================
            auto Uart::
rx_empty    () -> bool
            {
            return not anybit(m_uartx_base + UXSTA, 1<<URXDA);
            }

//uxbrg
//=============================================================================
            auto Uart::
baud_set    (uint32_t v) -> void
            {
            v = v < 110 ? 110 : v;        //minimum 110
            v = v > 921600 ? 921600 : v;  //maximum 921600
            m_uartx_baud = v;             //store
            //always use highspeed, simpler and works fine @ 24Mhz/921600
            //max clock and lowest baud also fits in uxbrg in highspeed
            v = baud_clk() / 4 / v;
            hispeed(true);
            //if v is 0 (clock too slow for desired baud),
            //just set to maximum baud for this clock
            val(m_uartx_base + UXBRG, v ? v-1 : 0);
            }

//called by clk_sel(), on()
//=============================================================================
            auto Uart::
baud_set    () -> void
            {
            //if baud not set, set it to 115200
            baud_set(m_uartx_baud ? m_uartx_baud : 115200);
            }

//=============================================================================
            auto Uart::
baud_clk    () -> uint32_t
            {
            CLKSEL e = (CLKSEL)((val(m_uartx_base)>>17) bitand
                CLKSEL_MASK);
            if(e == REFO1) return Osc::refo_freq();
            else if(e == FRC) return Osc::frcclk();
            return Osc::sysclk(); //pb/sys are the same
            }


//misc

// putc required for any derived class of Putc
// so we can change to any Putc device for printf use indirecly via
// via _mon_putc which printf ultimately will use (see Putc.hpp/cpp)
//=============================================================================
            auto Uart::
putc        (const char c) -> void
            {
            while(tx_full());
            write(c);
            }

//=============================================================================
            auto Uart::
puts        (const char* s) -> void
            {
            while(s && *s) putc(*s++);
            }

//=============================================================================
            auto Uart::
printf      (char const *fmt, ...) -> void
            {
            va_list args;
            va_start(args, fmt);
            sprintf(fmt, args);
            va_end(args);
            puts( sprintfbuf() );
            }

// with va_list already setup (used by Debug class)
//=============================================================================
            auto Uart::
printf      (char const *fmt, va_list args) -> void
            {
            sprintf(fmt, args);
            puts( sprintfbuf() );
            }

//=============================================================================
            auto Uart::
getc        () -> int
            {
            if( rx_empty() ) return -1;
            if( not rx_err() ) return read();
            rx_errclr(); //just reset buffer on any error
            return -1;
            }
