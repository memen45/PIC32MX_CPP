#pragma once

#include <cstdint>

struct Sys {

            //==== CFGCON ====

            static auto
bus_err     (bool) -> void;

            enum
BMXARB      : uint8_t { CPUHIGH, CPULOW, RROBIN };

            static auto
bus_mode    (BMXARB) -> void;

            static auto
ram_exec    (uint8_t) -> void;

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


            //==== ANCFG ====

            static auto
bgap_adc    (bool) -> void;

            static auto
bgap_comp   (bool) -> void;


            //==== UDID ====

            enum
UDID        : uint32_t { LOT = 0xBFC41840, DIE = 0xBFC4184C };

            static auto
udid        (UDID) -> uint64_t;

};

/*
udid - only 11 unique bytes - 9 unused
UDID1: FF917471 Lot Number H
UDID2: FF938000 Lot Number L
UDID3: FFFFFF23 Scribe
UDID4: FFFF0253 Die X coordinate
UDID5: FFFF0253 Die Y coordinate

udid function returns either LOT number or DIE info
UDID  lot: 0091747193800023 LOTH|LOTL|SCRIBE
UDID  die: 0000000002530253 DIEX|DIEY


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