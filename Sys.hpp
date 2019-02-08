#pragma once

#include <cstdint>
#include "Reg.hpp"

struct Sys : private Reg {

            //==== CFGCON ====

//            static auto
//bus_err     (bool) -> void;

            enum
BMXARB      : uint8_t { CPUHIGH, CPULOW, RROBIN };

            static auto
bus_mode    (BMXARB) -> void;

            static auto
jtag        (bool) -> void;

            //==== DEVID ====

            static auto
devid       () -> uint32_t;

            static auto
ver         () -> uint8_t;


            //==== SYSKEY ====

            static auto
lock        () -> void;

            static auto
lock        (uint8_t) -> void;

            static auto
unlock      () -> void;

            static auto
unlock_wait () -> uint8_t;


			//==== BMX ====
			
			static auto
flash_size	() -> uint32_t;

            //==== prefetch cache ====
    
            enum 
PREFEN      : uint8_t { 
            PF_EN = 0, PF_CACHE_ONLY, PF_NON_CACHE_ONLY, PF_ALL 
            };
            
            static auto
pcache      (PREFEN) -> void;
            
            static auto
waitstates  (uint32_t) -> void;
    
private:
            enum 
KSEG0_CACHE : uint8_t { 
            OFF = 2, ON = 3,
            };
            
            static auto
kseg0_cache_enable  (KSEG0_CACHE) -> void;
};

/*
udid - only 11 bytes used - 9 unused
UDID1: FF917471 Lot Number H
UDID2: FF938000 Lot Number L
UDID3: FFFFFF23 Scribe
UDID4: FFFF0253 Die X coordinate
UDID5: FFFF0253 Die Y coordinate
(appear to be bcd numbers)
not too useful, so just create a 'unique' 64 bit hash

hashed value returned from udid()-
UDID: 5cc0ece0d9c1d4a6



devid
PIC32MM0064GPM028 0x07708053 0b0000 0111 0111 0000 1000 0000 0101 0011 64/16
PIC32MM0128GPM028 0x07710053 0b0000 0111 0111 0001 0000 0000 0101 0011 128/16
PIC32MM0256GPM028 0x07718053 0b0000 0111 0111 0001 1000 0000 0101 0011 256/32

PIC32MM0064GPM036 0x0770A053 0b0000 0111 0111 0000 1010 0000 0101 0011
PIC32MM0128GPM036 0x07712053 0b0000 0111 0111 0001 0010 0000 0101 0011
PIC32MM0256GPM036 0x0771A053 0b0000 0111 0111 0001 1010 0000 0101 0011

PIC32MM0064GPM048 0x0772C053 0b0000 0111 0111 0010 1100 0000 0101 0011
PIC32MM0128GPM048 0x07734053 0b0000 0111 0111 0011 0100 0000 0101 0011
PIC32MM0256GPM048 0x0773C053 0b0000 0111 0111 0011 1100 0000 0101 0011

PIC32MM0064GPM064 0x0770E053 0b0000 0111 0111 0000 1110 0000 0101 0011
PIC32MM0128GPM064 0x07716053 0b0000 0111 0111 0001 0110 0000 0101 0011
PIC32MM0256GPM064 0x0771E053 0b0000 0111 0111 0001 1110 0000 0101 0011
*/
