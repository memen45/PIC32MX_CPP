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
    (hardware 'forced' alignment, but may not be what is wanted if user error)
*/

struct Nvm {

            enum
MEM         : uint32_t {
            BASEFLASH = 0x9D000000, //kseg0
            PAGESIZE = 512*4,       //512words, 2048bytes
            ROWSIZE = 64*4,         //64words, 256bytes
            };

            static auto
mem_size    () -> uint32_t;

            static auto
write_2word (uint32_t, uint32_t, uint32_t) -> uint8_t;

            static auto
write_row   (uint32_t, uint32_t) -> uint8_t; //src,dst

            static auto
page_erase  (uint32_t) -> uint8_t;

            static auto
write_protect (uint32_t, bool) -> void; //true=lock until reset

            enum
BOOTP       : uint16_t { PAGE0 = 1<<8, PAGE1 = 1<<9, PAGE2 = 1<<10 };

            static auto
boot_protect (BOOTP, bool) -> void; //true=lock until reset

};

/*
pic32mm0256gpm064 series-

mem pins devid
64  28 0x07708053   000 01 000
128 28      10      000 10 000
256 28      18      000 11 000

64  36 0x0770A053   000 01 010
128 36      12      000 10 010
256 36      1A      000 11 010

64  48 0x0772C053   001 01 100
128 48      34      001 10 100
256 48      3C      001 11 100

64  64 0x0770E053   000 01 110
128 28      16      000 10 110
256 28      1E      000 11 110
                        ||
devid bit<16:15>-------//  -> 1=64K, 2=128K, 3=256K

*/
