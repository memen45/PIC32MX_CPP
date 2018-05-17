#include "Uart.hpp"
#include "Reg.hpp"
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

//setup pins manually
//=============================================================================
            Uart::
Uart        (UARTX e)
            :
            m_uartx_base((vu32ptr)U1MODE + ((e bitand 3) * UARTX_SPACING)),
            m_uartx_tx(*((vu32ptr)U1MODE + ((e bitand 3) *
                UARTX_SPACING) + UXTXREG)),
            m_uartx_rx(*((vu32ptr)U1MODE + ((e bitand 3) *
                UARTX_SPACING) + UXRXREG)),
            m_uartx_baud(0)
            {
            }

//specify pins
//=============================================================================
            Uart::
Uart        (UARTX e, Pins::RPN tx, Pins::RPN rx, uint32_t baud)
            :
            Uart(e)
            {
            //UART1 is fixed pins, no pps
            if(e == Uart::UART2){
                Pins r(rx); r.pps_in(Pins::U2RX);
                Pins t(tx); t.pps_out(Pins::U2TX);
            }
            else if(e == Uart::UART3){
                Pins r(rx); r.pps_in(Pins::U3RX);
                Pins t(tx); t.pps_out(Pins::U3TX);
            }
            m_uartx_baud = baud;
            baud_set();
            rx_on(true);
            tx_on(true);
            }

//tx or rx only
//=============================================================================
            Uart::
Uart        (UARTX e, Pins::RPN trx, uint32_t baud)
            :
            Uart(e)
            {
            //UART1 is fixed pins, no pps
            if(e == Uart::UART2){
                if(e bitand (1<<2)){
                    Pins t(trx); t.pps_out(Pins::U2TX);
                    tx_on(true);
                } else {
                    Pins r(trx); r.pps_in(Pins::U2RX);
                    rx_on(true);
                }
            }
            else if(e == Uart::UART3){
                if(e bitand (1<<2)){
                    Pins t(trx); t.pps_out(Pins::U3TX);
                    tx_on(true);
                } else {
                    Pins r(trx); r.pps_in(Pins::U3RX);
                    rx_on(true);
                }
            }
            m_uartx_baud = baud;
            baud_set();
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
            Reg::setbit(m_uartx_base, 1<<SLPEN, not tf);
            }

//=============================================================================
            auto Uart::
active      () -> bool
            {
            return Reg::anybit(m_uartx_base, 1<<ACTIVE);
            }

//=============================================================================
            auto Uart::
clk_sel     (CLKSEL e) -> void
            {
            Reg::clrbit(m_uartx_base, CLKSEL_MASK<<CLKSEL_SHIFT);
            Reg::setbit(m_uartx_base, e<<CLKSEL_SHIFT);
            baud_set();
            }

//=============================================================================
            auto Uart::
oflow_stop  (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<OVFDIS, not tf);
            }

//=============================================================================
            auto Uart::
on          (bool tf) -> void
            {
            baud_set(); //in case not set
            Reg::setbit(m_uartx_base, 1<<ON, tf);
            }

//=============================================================================
            auto Uart::
stop_idle   (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<SIDL, tf);
            }

//=============================================================================
            auto Uart::
irda        (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<IREN, tf);
            }

//=============================================================================
            auto Uart::
rts_mode    (RTSMODE e) -> void
            {
            Reg::setbit(m_uartx_base, 1<<RTSMD, e);
            }

//=============================================================================
            auto Uart::
wake        (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<WAKE, tf);
            }

//=============================================================================
            auto Uart::
loopback    (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<LPBACK, tf);
            }

//=============================================================================
            auto Uart::
autobaud    (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<ABAUD, tf);
            }

//=============================================================================
            auto Uart::
rx_pol      (RXPOL e) -> void
            {
            Reg::setbit(m_uartx_base, 1<<RXINV, e);
            }

//=============================================================================
            auto Uart::
hispeed     (bool tf) -> void
            {
            Reg::setbit(m_uartx_base, 1<<BRGH, tf);
            }

//=============================================================================
            auto Uart::
mode        (MODESEL e) -> void
            {
            Reg::clrbit(m_uartx_base, MODE_MASK<<MODE_SHIFT);
            Reg::setbit(m_uartx_base, e<<MODE_SHIFT);
            }

//uxsta
//=============================================================================
            auto Uart::
rx_mask     (uint8_t v) -> void
            {
            Reg::val((vu8ptr)m_uartx_base + (UXSTA * 4) + 3, v);
            }

//=============================================================================
            auto Uart::
rx_addr     (uint8_t v) -> void
            {
            Reg::val((vu8ptr)m_uartx_base + (UXSTA * 4) + 2, v);
            }

//=============================================================================
            auto Uart::
tx_irq      (UTXISEL e) -> void
            {
            Reg::clrbit(m_uartx_base + UXSTA, UTXISEL_MASK<<UTXISEL_SHIFT);
            Reg::setbit(m_uartx_base + UXSTA, e<<UTXISEL_SHIFT);
            }

//=============================================================================
            auto Uart::
tx_pol      (RXPOL e) -> void
            {
            bool b = Reg::anybit(m_uartx_base, 1<<IREN) ? not e : e;
            Reg::setbit(m_uartx_base + UXSTA, 1<<UTXINV, b);
            }

//=============================================================================
            auto Uart::
rx_on       (bool tf) -> void
            {
            Reg::setbit(m_uartx_base + UXSTA, 1<<URXEN, tf);
            }

//=============================================================================
            auto Uart::
tx_break    () -> void
            {
            Reg::setbit(m_uartx_base + UXSTA, 1<<UTXBRK);
            }

//=============================================================================
            auto Uart::
tx_on       (bool tf) -> void
            {
            Reg::setbit(m_uartx_base + UXSTA, 1<<UTXEN, tf);
            }

//=============================================================================
            auto Uart::
tx_full     () -> bool
            {
            return Reg::anybit(m_uartx_base + UXSTA, 1<<UTXBF);
            }

//=============================================================================
            auto Uart::
tx_done     () -> bool
            {
            return Reg::anybit(m_uartx_base + UXSTA, 1<<TRMT);
            }

//=============================================================================
            auto Uart::
rx_irq      (URXISEL e) -> void
            {
            Reg::clrbit(m_uartx_base + UXSTA, URXISEL_MASK<<URXISEL_SHIFT);
            Reg::setbit(m_uartx_base + UXSTA, e<<URXISEL_SHIFT);
            }

//=============================================================================
            auto Uart::
rx_addren   (bool tf) -> void
            {
            Reg::setbit(m_uartx_base + UXSTA, 1<<ADDEN, tf);
            }

//=============================================================================
            auto Uart::
rx_busy     () -> bool
            {
            return not Reg::anybit(m_uartx_base + UXSTA, 1<<RIDLE);
            }

//=============================================================================
            auto Uart::
rx_perr     () -> bool
            {
            return Reg::anybit(m_uartx_base + UXSTA, 1<<PERR);
            }

//=============================================================================
            auto Uart::
rx_ferr     () -> bool
            {
            return Reg::anybit(m_uartx_base + UXSTA, 1<<FERR);
            }

//=============================================================================
            auto Uart::
rx_oerr     () -> bool
            {
            return Reg::anybit(m_uartx_base + UXSTA, 1<<OERR);
            }

//=============================================================================
            auto Uart::
rx_empty    () -> bool
            {
            return not Reg::anybit(m_uartx_base + UXSTA, 1<<URXDA);
            }

//uxbrg
//=============================================================================
            auto Uart::
baud_set    (uint32_t v) -> void
            {
            m_uartx_baud = v;
            uint32_t bclk = baud_clk();
            v = (bclk / 16 / (m_uartx_baud / 100));
            if( ((v / 100 * 100) / (v % 100)) < 50 ){ //<50 = >2% error,
                v *= 4; //so switch to hispeed
                hispeed(true);
            } else { hispeed(false); }
            Reg::val(m_uartx_base + UXBRG, (v/100)-1);
            }
// 115200 8MHz
//8000000 / 16 / 1152 = 434,
// (434 / 100 * 100) / (434 % 100) = 11 = 8.5%error -> need hispeed
//8000000 / 4 / 1152 = 1736,
// (1736 / 100 * 100) / (1736 % 100) = 47 = 2.1%error -> ok (best we can get)
// 115200 24MHz
//24000000 / 16 / 1152 = 1302,
// (1302 / 100 * 100) / (1302 % 100) = 650 = 0.15%error -> ok
// 230400 24MHz
//24000000 / 16 / 2304 = 651,
// (651 / 100 * 100) / (651 % 100) = 11 = 8.5%error -> need hispeed
//24000000 / 4 / 2304 = 2604,
// (2604 / 100 * 100) / (2604 % 100) = 650 = 0.15%error -> ok

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
            CLKSEL e = (CLKSEL)((Reg::val(m_uartx_base)>>17) bitand
                CLKSEL_MASK);
            if(e == REFO1) return Osc::refo_freq();
            else if(e == FRC) return Osc::frcclk();
            return Osc::sysclk(); //pb/sys are the same
            }


//misc
//=============================================================================
            auto Uart::
putchar     (const char c) -> void
            {
            while(tx_full());
            write(c);
            }

//=============================================================================
            auto Uart::
puts        (const char* s) -> void
            {
            while(s && *s) putchar(*s++);
            }

//=============================================================================
            auto Uart::
getchar     () -> int
            {
            return rx_empty() ? -1 : read();
            }
