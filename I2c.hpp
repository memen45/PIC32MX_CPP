#pragma once

#include <cstdint>
#include "Reg.hpp"

//I2c 1/2/3

struct I2c  {

    //instantiate I2c with i2c number
    enum I2CX { I2C1 = 0, I2C2, I2C3 };

    /*constexpr*/ I2c(I2CX);

    //I2C1CON
    enum HOLDTIM : bool { NS100 = 0, NS300 };

    void            irq_stop        (bool);             //irq enable on stop
    void            irq_start       (bool);             //irq enable on start
    void            overwrite       (bool);
    void            hold_time       (HOLDTIM);
    void            irq_collision   (bool);
    void            on              (bool);
    void            stop_idle       (bool);
    void            clk_release     (bool);
    void            strict          (bool);
    void            addr_10bit      (bool);
    void            slew_rate       (bool); //0=true
    void            smb_levels      (bool);
    void            irq_gencall     (bool);
    void            clk_stretch     (bool);
    void            ack             (bool);
    void            rx              (bool);
    void            stop            (bool);
    void            repstart        (bool);
    void            start           (bool);

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
    bool            stat            (STAT);
    void            buscol_clr      ();
    void            txcol_clr       ();
    void            rxoflow_clr     ();

    //I2CXADDR
    void            addr            (uint16_t);

    //I2CXMSK
    void            addr_mask       (uint16_t);

    //I2CXBRG
    enum I2CSPEED : uint32_t {
        KHZ100 = 100000, KHZ400 = 400000, MHZ1 = 1000000
    };

    void            speed           (I2CSPEED);
    void            brg             (uint16_t);

    //I2CXTRN
    void            write           (uint8_t);

    //I2CXRCV
    uint8_t         read            ();

    private:

    Reg r;

    enum :uint32_t {
        I2C1CON = 0xBF801500, I2CX_SPACING = 0x40, //spacing in words
            PCIE = 1<<22,
            SCIE = 1<<21,
            BOEN = 1<<20,
            SDAHT = 1<<19,
            SBCDE = 1<<18,
            ON = 1<<15,
            SIDL = 1<<13,
            SCLREL = 1<<12,
            STRICT = 1<<11,
            A10M = 1<<10,
            DISSLW = 1<<9,
            SMEN = 1<<8,
            GCEN = 1<<7,
            STREN = 1<<6,
            ACKDT = 1<<5,
            ACKEN = 1<<4,
            RCEN = 1<<3,
            PEN = 1<<2,
            RSEN = 1<<1,
            SEN = 1<<0,
        I2CXSTAT = 4, //offset from I2xCON in words
        I2CXADDR = 8,
        I2CXMSK = 12,
        I2CXBRG = 16,
        I2CXTRN = 20,
        I2CXRCV = 24
    };

    volatile uint32_t* m_i2cx_con;
    I2CSPEED m_speed;

};
