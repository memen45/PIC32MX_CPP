#pragma once

//Nvm

#include <cstdint>

/*
    all done from flash, so check of wr for completion is not made
    (cpu is halted until done, so no need)

    return values
    0= no error, 1= low voltage error, 2= write error

    no check is made for address alignment- if incorrect the address will be
    masked by the register used and an nvm op will take place on the register
   'forced' alignment
*/

struct Nvm {

    enum MEM : uint32_t {
        BASEMEM = 0x9D000000,
        PAGESIZE = 512*4, //512words, 2048bytes
        ROWSIZE = 64*4, //64words, 256bytes

        MAXMEM64 = PAGESIZE*32-1,
        MAXPAGE64 = 31, //0-31
        MAXROW64 = 255, //0-255

        MAXMEM128 = PAGESIZE*64-1,
        MAXPAGE128 = 63, //0-63
        MAXROW128 = 511, //0-511

        MAXMEM256 = PAGESIZE*128-1,
        MAXPAGE256 = 127, //0-127
        MAXROW256 = 1023, //0-1023

    };

    static auto
    write_2word (uint32_t, uint32_t, uint32_t) -> uint8_t;

    static auto
    write_row (uint32_t, uint32_t) -> uint8_t; //src,dst

    static auto
    page_erase (uint32_t) -> uint8_t;

    static auto
    write_protect (uint32_t, bool) -> void; //true=lock until reset

    enum BOOTP : uint16_t { PAGE0 = 1<<8, PAGE1 = 1<<9, PAGE2 = 1<<10 };
    static auto
    boot_protect (BOOTP, bool) -> void; //true=lock until reset

};
