#pragma once

#include <cstdint>

//Dma 0/1/2/3

struct Dma {

    //instantiate Dma with dma number
    enum DMAX { DMA0, DMA1, DMA2, DMA3 };
    Dma(DMAX);

    //DCHXCON
    bool            busy            ();

    enum CHCHAIN : uint8_t { TOHIGHER, TOLOWER, CHAINOFF };
    void            chain           (CHCHAIN);

    void            on              (bool);
    void            evt_always      (bool);
    void            auto_en         (bool);
    bool            evt             ();

    enum CHPRI : uint8_t { PRI0, PRI1, PRI2, PRI3 };
    void            priority        (CHPRI);

    //DCHXECON
    enum IRQ : uint8_t { IRQOFF = 255 };
    void            irq_abort       (uint8_t);
    void            irq_start       (uint8_t);
    void            start           ();
    void            abort           ();
    void            abort_match     (bool);

    //DCHXINT
    enum IRQENF : uint8_t { //byte positions, use reg byte acccess
        SRCDONE = 1<<7, SRCHALF = 1<<6, DSTDONE = 1<<5, DSTHALF = 1<<4,
        BLKDONE = 1<<3, CELLDONE = 1<<2, XFERABORT = 1<<1, ADDRERR = 1<<0
    };
    void            irq             (IRQENF, bool);
    bool            flag            (IRQENF);
    void            flag_clr        (IRQENF);

    //DCHXSSA
    void            sstart_addr     (uint32_t);

    //DCHXDSA
    void            dstart_addr     (uint32_t);

    //DCHXSSIZ
    void            ssize           (uint16_t);

    //DCHXDSIZ
    void            dsize           (uint16_t);

    //DCHXSPTR
    void            spointer        (uint16_t);

    //DCHXDPTR
    void            dpointer        (uint16_t);

    //DCHXCSIZ
    void            csize           (uint16_t);

    //DCHXCPTR
    uint16_t        cpointer        ();

    //DCHXDAT
    void            pattern         (uint8_t);

    //common for all instances
    //DMACON
    static void     all_on          (bool);             //dma on/off
    static void     all_suspend     (bool);             //dma suspend
    static bool     all_suspend     ();                 //dma suspended?
    static bool     any_busy        ();                 //dma busy

    //DMASTAT
    static bool     last_rd         ();                 //last dma was read
                                                        //(else was write)
    static uint8_t  last_ch         ();                 //last dma channel

    //DMAADDR
    static uint32_t last_addr       ();                 //last dma address

    //DCRCCON
    enum CRCBYTO : uint8_t { SAME, REVERSE, SWAPH, SWAPB };
    static void     crc_byto        (CRCBYTO);

    enum CRCBITO : bool { MSB, LSB };
    static void     crc_bito        (CRCBITO);

    static void     crc_polyn       (uint8_t);
    static void     crc_on          (bool);
    static void     crc_append      (bool);

    enum CRCTYPE : bool { LFSR, IP };
    static void     crc_type        (CRCTYPE);

    static void     crc_ch          (uint8_t);

    //DCRCDATA
    static void     crc_seed        (uint32_t);
    static uint32_t crc_data        ();

    //DCRCXOR
    static void     crc_xor         (uint32_t);

    private:

    volatile uint32_t* m_dmax_con;

};
