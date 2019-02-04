#include "I2c.hpp"
#include "Osc.hpp"

enum :uint32_t {
I2CX_SPACING = 0x40, //spacing in words
I2C1CON = 0xBF801500,
    PCIE = 22,
    SCIE = 21,
    BOEN = 20,
    SDAHT = 19,
    SBCDE = 18,
    ON = 15,
    SIDL = 13,
    SCLREL = 12,
    STRICT = 11,
    A10M = 10,
    DISSLW = 9,
    SMEN = 8,
    GCEN = 7,
    STREN = 6,
    ACKDT = 5,
    ACKEN = 4,
    RCEN = 3,
    PEN = 2,
    RSEN = 1,
    SEN = 0,
I2CXSTAT = 4, //offset from I2xCON in words
I2CXADDR = 8,
I2CXMSK = 12,
I2CXBRG = 16,
I2CXTRN = 20,
I2CXRCV = 24
};

//=============================================================================
            I2c::
I2c         (I2CX e)
            : m_i2cx_con((volatile uint32_t*)I2C1CON + (e * I2CX_SPACING)),
                m_speed(KHZ100)
            {
            }

//I2C1CON
//=============================================================================
            auto I2c::
irq_stop    (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<PCIE, tf);
            }

//=============================================================================
            auto I2c::
irq_start   (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<SCIE, tf);
            }

//=============================================================================
            auto I2c::
overwrite   (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<BOEN, tf);
            }

//=============================================================================
            auto I2c::
hold_time   (HOLDTIM e) -> void
            {
            setbit(m_i2cx_con, 1<<SDAHT, e);
            }

//=============================================================================
            auto I2c::
irq_collision (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<SBCDE, tf);
            }

//=============================================================================
            auto I2c::
on          (bool tf) -> void
            {
            //always set brg in case clock changed
            //or speed not changed since init
            speed(m_speed);
            setbit(m_i2cx_con, 1<<ON, tf);
            }

//=============================================================================
            auto I2c::
stop_idle   (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<SIDL, tf);
            }

//=============================================================================
            auto I2c::
clk_release (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<SCLREL, tf);
            }

//=============================================================================
            auto I2c::
strict      (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<STRICT, tf);
            }

//=============================================================================
            auto I2c::
addr_10bit  (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<A10M, tf);
            }

//=============================================================================
            auto I2c::
slew_rate   (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<DISSLW, not tf);
            }

//=============================================================================
            auto I2c::
smb_levels  (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<SMEN, tf);
            }

//=============================================================================
            auto I2c::
irq_gencall (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<GCEN, tf);
            }

//=============================================================================
            auto I2c::
clk_stretch (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<STREN, tf);
            }

//=============================================================================
            auto I2c::
ack         (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<ACKDT, not tf); //0=ACK,1=NACK
            setbit(m_i2cx_con, 1<<ACKEN);
            }

//=============================================================================
            auto I2c::
rx          (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<RCEN, tf);
            }

//=============================================================================
            auto I2c::
stop        (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<PEN, tf);
            }

//=============================================================================
            auto I2c::
repstart    (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<RSEN, tf);
            }

//=============================================================================
            auto I2c::
start       (bool tf) -> void
            {
            setbit(m_i2cx_con, 1<<SEN, tf);
            }

//I2CXSTAT
//=============================================================================
            auto I2c::
stat        (STAT e) -> bool
            {
            return anybit(m_i2cx_con + I2CXSTAT, e);
            }

//=============================================================================
            auto I2c::
buscol_clr  () -> void
            {
            clrbit(m_i2cx_con + I2CXSTAT, BUSCOL);
            }

//=============================================================================
            auto I2c::
txcol_clr   () -> void
            {
            clrbit(m_i2cx_con + I2CXSTAT, TXCOL);
            }

//=============================================================================
            auto I2c::
rxoflow_clr () -> void
            {
            clrbit(m_i2cx_con + I2CXSTAT, RXOFLOW);
            }

//I2CXADDR
//=============================================================================
            auto I2c::
addr        (uint16_t v) -> void
            {
            val(m_i2cx_con + I2CXADDR, v);
            }

//I2CXMSK
//=============================================================================
            auto I2c::
addr_mask   (uint16_t v) -> void
            {
            val(m_i2cx_con + I2CXMSK, v);
            }

//I2CXBRG
//=============================================================================
            auto I2c::
speed       (I2CSPEED e) -> void
            {
            uint32_t sck = e * 2;
            uint32_t clk = Osc::sysclk()<<6;
            clk = clk / sck - clk / 9615384; //1/9615384=104ns=Tpgd
            brg((clk>>6) - 2);
            m_speed = e;
            }

//=============================================================================
            auto I2c::
brg         (uint16_t v) -> void
            {
            if(v < 2) v = 2; //0,1 not allowed
            val(m_i2cx_con + I2CXBRG, v);
            }

//I2CXTRN
//=============================================================================
            auto I2c::
write       (uint8_t v) -> void
            {
            val(m_i2cx_con + I2CXTRN, v);
            }

//I2CXRCV
//=============================================================================
            auto I2c::
read        () -> uint8_t
            {
            return val8(m_i2cx_con + I2CXRCV);
            }
