#pragma once

#include <cstdint>
#include "Reg.hpp"

//Dma 0/1/2/3

struct Dma {

    //instantiate Dma with dma number
    enum DMAX { DMA0 = 0, DMA1, DMA2, DMA3 };

    /*constexpr*/ Dma(DMAX);

    //DCHXCON
    enum CHCHAIN : uint8_t { TOHIGHER = 0, TOLOWER = 1, CHAINOFF = 2 };
    enum CHPRI : uint8_t { PRI0 = 0, PRI1, PRI2, PRI3 };

    bool            busy            ();
    void            chain           (CHCHAIN);
    void            on              (bool);
    void            evt_always      (bool);
    void            auto_en         (bool);
    bool            evt             ();
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
    enum CRCBYTO : uint8_t { SAME = 0, REVERSE, SWAPH, SWAPB };
    enum CRCBITO : bool { MSB = 0, LSB };
    enum CRCTYPE : bool { LFSR = 0, IP };

    static void     crc_byto        (CRCBYTO);
    static void     crc_bito        (CRCBITO);
    static void     crc_polyn       (uint8_t);
    static void     crc_on          (bool);
    static void     crc_append      (bool);
    static void     crc_type        (CRCTYPE);
    static void     crc_ch          (uint8_t);

    //DCRCDATA
    static void     crc_seed        (uint32_t);
    static uint32_t crc_data        ();

    //DCRCXOR
    static void     crc_xor         (uint32_t);

    private:

    static Reg r;

    enum {
        DMACON = 0xBF808900,
            ON = 1<<15,
            SUSPEND = 1<<12,
            DMABUSY = 1<<11,
        DMASTAT = 0xBF808910,
            RDWR = 1<<3,
            DMACH_SHIFT = 0, DMACH_CLR = 7,
        DMAADDR = 0xBF808920,
        DCRCCON = 0xBF808930,
            BYTO_SHIFT = 28, BYTO_CLR = 3,
            WBO = 1<<27,
            BITO = 1<<24,
            PLEN_SHIFT = 8, PLEN_CLR = 31,
            CRCEN = 1<<7,
            CRCAPP = 1<<6,
            CRCTYP = 1<<5,
            CRCCH_SHIFT = 0, CRCCH_CLR = 7,
        DCRCDATA = 0xBF808940,
        DCRCXOR = 0xBF808950,
        DCH0CON = 0xBF808960, DCH_SPACING = 0x30, //spacing in words
            CHBUSY = 1<<15,
            CHCHNS = 1<<8,
            CHEN = 1<<7,
            CHAED = 1<<6,
            CHCHN = 1<<5,
            CHAEN = 1<<4,
            CHEDET = 1<<2,
            CHPRI_SHIFT = 0, CHPRI_CLR = 3,
        //register offsets from base in words
        //(m_dmax_con is pointer, so addition will be in words)
        DCHXECON = 4,
            CHAIRQ_SHIFT = 16, CHAIRG_CLR = 255, //can use byte access
            CHSIRQ_SHIFT = 8, CHSIRQ_CLR = 255, //can use byte access
            CFORCE = 1<<7,
            CABORT = 1<<6,
            PATEN = 1<<5,
            SIRQEN = 1<<4,
            AIRQEN = 1<<3,
        DCHXINT = 8,
            CHSDIE = 1<<23,
            CHSHIE = 1<<22,
            CHDDIE = 1<<21,
            CHDHIE = 1<<20,
            CHBCIE = 1<<19,
            CHCCIE = 1<<18,
            CHTAIE = 1<<17,
            CHERIE = 1<<16,
            CHSDIF = 1<<7,
            CHSHIF = 1<<6,
            CHDDIF = 1<<5,
            CHDHIF = 1<<4,
            CHBCIF = 1<<3,
            CHCCIF = 1<<2,
            CHTAIF = 1<<1,
            CHERIF = 1<<0,
        DCHXSSA = 12,
        DCHXDSA = 16,
        DCHXSSIZ = 20,
        DCHXDSIZ = 24,
        DCHXSPTR = 28,
        DCHXDPTR = 32,
        DCHXCSIZ = 36,
        DCHXCPTR = 40,
        DCHXDAT = 44
    };

    volatile uint32_t* m_dmax_con;

    using vbyte_ptr = volatile uint8_t*;            //access reg as bytes

};
