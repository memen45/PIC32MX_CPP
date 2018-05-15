#pragma once

#include <cstdint>

//I2c 1/2/3

struct I2c  {

    //instantiate I2c with i2c number
    enum I2CX { I2C1, I2C2, I2C3 };
    I2c(I2CX);

    //I2C1CON

    auto
    irq_stop (bool) -> void;             //irq enable on stop

    auto
    irq_start (bool) -> void;             //irq enable on start

    auto
    overwrite (bool) -> void;

    enum HOLDTIM : bool { NS100, NS300 };
    auto
    hold_time (HOLDTIM) -> void;

    auto
    irq_collision (bool) -> void;

    auto
    on (bool) -> void;

    auto
    stop_idle (bool) -> void;

    auto
    clk_release (bool) -> void;

    auto
    strict (bool) -> void;

    auto
    addr_10bit (bool) -> void;

    auto
    slew_rate (bool) -> void; //0=true

    auto
    smb_levels (bool) -> void;

    auto
    irq_gencall (bool) -> void;

    auto
    clk_stretch (bool) -> void;

    auto
    ack (bool) -> void;

    auto
    rx (bool) -> void;

    auto
    stop (bool) -> void;

    auto
    repstart (bool) -> void;

    auto
    start (bool) -> void;

    //I2CXSTAT

    enum STAT : uint16_t {
        ACK = 1<<15,
        TXBUSY = 1<<14,
        ACKTIM = 1<<13,
        BUSCOL = 1<<10,
        GENCALL = 1<<9,
        ADD10 = 1<<8,
        TXCOL = 1<<7,
        RXOFLOW = 1<<6,
        DA = 1<<5,
        P = 1<<4,
        S = 1<<3,
        RW = 1<<2,
        RXFULL = 1<<1,
        TXFULL = 1<<0,
    };
    auto
    stat (STAT) -> bool;

    auto
    buscol_clr () -> void;

    auto
    txcol_clr () -> void;

    auto
    rxoflow_clr () -> void;

    //I2CXADDR

    auto
    addr (uint16_t) -> void;

    //I2CXMSK

    auto
    addr_mask (uint16_t) -> void;

    //I2CXBRG

    enum I2CSPEED : uint32_t {
        KHZ100 = 100000, KHZ400 = 400000, MHZ1 = 1000000
    };
    auto
    speed (I2CSPEED) -> void;

    auto
    brg (uint16_t) -> void;

    //I2CXTRN

    auto
    write (uint8_t) -> void;

    //I2CXRCV

    auto
    read () -> uint8_t;


    private:

    volatile uint32_t* m_i2cx_con;
    I2CSPEED m_speed;

};
