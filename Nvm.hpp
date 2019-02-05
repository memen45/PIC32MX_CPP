#pragma once

//Nvm

#include <cstdint>
#include "Reg.hpp"

/*
    all done from flash, so check of wr for completion is not made
    (cpu is halted until done, so no need)
    if need to keep cpu running, create your own function(s) in ram

    return values
    0= no error, 1= low voltage error, 2= write error

    no check is made for address alignment- if incorrect the address will be
    masked by the register/op used and an nvm op will take place
    (hardware 'forced' alignment, but may not be what is wanted if user error)
*/

struct Nvm : private Reg {

            enum
MEM         : uint32_t {
            BASEFLASH = 0x9D000000, //kseg0
			PAGESIZE = 1024 * 4,	// 1024 words, 4096 bytes
			ROWSIZE = 128 * 4,		// 128 words, 512 bytes
            };

            static auto
mem_size    () -> uint32_t;

            static auto
write_word  (uint32_t, uint32_t) -> uint8_t;

            static auto
write_row   (uint32_t, uint32_t) -> uint8_t; //src,dst

            static auto
page_erase  (uint32_t) -> uint8_t;

};
