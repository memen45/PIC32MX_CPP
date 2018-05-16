#pragma once

#include <cstdint>

//Dma 0/1/2/3

struct Dma {

        enum
    DMAX { DMA0, DMA1, DMA2, DMA3 };

    Dma(DMAX);      //instantiate Dma with dma number

    //DCHXCON

        auto
    busy () -> bool;

        enum
    CHCHAIN : uint8_t { TOHIGHER, TOLOWER, CHAINOFF };

        auto
    chain (CHCHAIN) -> void;

        auto
    on (bool) -> void;

        auto
    evt_always (bool) -> void;

        auto
    auto_en (bool) -> void;

        auto
    evt () -> bool;

        enum
    CHPRI : uint8_t { PRI0, PRI1, PRI2, PRI3 };

        auto
    priority (CHPRI) -> void;

    //DCHXECON

        enum
    IRQ : uint8_t { IRQOFF = 255 };

        auto
    irq_abort (uint8_t) -> void;

        auto
    irq_start (uint8_t) -> void;

        auto
    start () -> void;

        auto
    abort () -> void;

        auto
    abort_match (bool) -> void;

    //DCHXINT

        enum
    IRQENF : uint8_t { //byte positions, use reg byte acccess
        SRCDONE = 1<<7, SRCHALF = 1<<6, DSTDONE = 1<<5, DSTHALF = 1<<4,
        BLKDONE = 1<<3, CELLDONE = 1<<2, XFERABORT = 1<<1, ADDRERR = 1<<0
        };

        auto
    irq (IRQENF, bool) -> void;

        auto
    flag (IRQENF) -> bool;

        auto
    flag_clr (IRQENF) -> void;

    //DCHXSSA

        auto
    sstart_addr (uint32_t) -> void;

    //DCHXDSA

        auto
    dstart_addr (uint32_t) -> void;

    //DCHXSSIZ

        auto
    ssize (uint16_t) -> void;

    //DCHXDSIZ

        auto
    dsize (uint16_t) -> void;

    //DCHXSPTR

        auto
    spointer (uint16_t) -> void;

    //DCHXDPTR

        auto
    dpointer (uint16_t) -> void;

    //DCHXCSIZ

        auto
    csize (uint16_t) -> void;

    //DCHXCPTR

        auto
    cpointer () -> uint16_t;

    //DCHXDAT

        auto
    pattern (uint8_t) -> void;

    //common for all instances
    //DMACON

        static auto
    all_on (bool) -> void;          //dma on/off

        static auto
    all_suspend (bool) -> void;     //dma suspend

        static auto
    all_suspend () -> bool;         //dma suspended?

        static auto
    any_busy () -> bool;            //dma busy

    //DMASTAT

        static auto
    last_rd () -> bool;             //last dma was read
                                    //(else was write)
        static auto
    last_ch () -> uint8_t;          //last dma channel

    //DMAADDR

        static auto
    last_addr () -> uint32_t;       //last dma address

    //DCRCCON

        enum
    CRCBYTO : uint8_t { SAME, REVERSE, SWAPH, SWAPB };

        static auto
    crc_byto (CRCBYTO) -> void;

        enum
    CRCBITO : bool { MSB, LSB };

        static auto
    crc_bito (CRCBITO) -> void;

        static auto
    crc_polyn (uint8_t) -> void;

        static auto
    crc_on (bool) -> void;

        static auto
    crc_append (bool) -> void;

        enum
    CRCTYPE : bool { LFSR, IP };

        static auto
    crc_type (CRCTYPE) -> void;

        static auto
    crc_ch (uint8_t) -> void;

    //DCRCDATA

        static auto
    crc_seed (uint32_t) -> void;

        static auto
    crc_data () -> uint32_t;

    //DCRCXOR

        static auto
    crc_xor (uint32_t) -> void;


    private:

    volatile uint32_t* m_dmax_con;

};
