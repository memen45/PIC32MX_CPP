#pragma once

#include <cstdint>

//I2c 1/2/3

struct I2c  {

    //instantiate I2c with i2c number
    enum I2CX { I2C1, I2C2, I2C3 };
    I2c(I2CX);

    //I2C1CON
    void            irq_stop        (bool);             //irq enable on stop
    void            irq_start       (bool);             //irq enable on start
    void            overwrite       (bool);

    enum HOLDTIM : bool { NS100, NS300 };
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

    volatile uint32_t* m_i2cx_con;
    I2CSPEED m_speed;

};
