#pragma once

//Nvm

#include <cstdint>

/*
    all done from flash, so check of wr for completion is not made
    (cpu is halted until done, no need)

    return values
    0= no error, 1= low voltage error, 2= write error
*/

struct Nvm {

    enum MEM : uint32_t {
        BASEMEM = 0x9D000000,
        MAXMEM64 = 0xFFFF,
        MAXMEM128 = 0x1FFFF,
        MAXMEM256 = 0x3FFFF,
        PAGESIZE = 512*4, //512words, 2048bytes
        ROWSIZE = 64*4, //64words, 256bytes
        MAXPAGE256 = ((MAXMEM256+1)/PAGESIZE-1), //0-127
        MAXPAGE128 = ((MAXMEM128+1)/PAGESIZE-1), //0-63
        MAXPAGE64 = ((MAXMEM64+1)/PAGESIZE-1), //0-31
        MAXROW256 = ((MAXMEM256+1)/ROWSIZE-1), //0-1023
        MAXROW128 = ((MAXMEM128+1)/ROWSIZE-1), //0-511
        MAXROW64 = ((MAXMEM64+1)/ROWSIZE-1), //0-255
    };

    static uint8_t  write_dword     (uint32_t*, uint32_t, uint32_t);
    static uint8_t  write_row       (uint32_t*, uint32_t); //src,dst
    static uint8_t  page_erase      (uint32_t);
    static void     write_protect   (uint32_t, bool); //true=lock until reset

    enum BOOTP : uint16_t { PAGE0 = 1<<8, PAGE1 = 1<<9, PAGE2 = 1<<10 };
    static void     boot_protect    (BOOTP, bool); //true=lock until reset

    private:

    static uint8_t  unlock      ();
    static void     lock        (uint8_t);
    static void     do_wr       ();
    static void     do_op       (uint8_t);
    static void     address     (uint32_t);
    static uint8_t  error       ();
    static void     write_nop   ();

};
