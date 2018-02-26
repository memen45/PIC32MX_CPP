#pragma once

#include <cstdint>

class Sys {

    public:

    //cfgcon
    static void     bus_err     (bool);

    enum BMXARB : uint8_t { CPUHIGH = 0, CPULOW, RROBIN };
    static void     bus_mode    (BMXARB);

    static void     jtag        (bool);

    //devid
    static uint32_t devid       ();
    static uint8_t  ver         ();

    //syskey
    static void     lock        ();
    static void     lock        (uint8_t);
    static void     unlock      ();
    static uint8_t  unlock_wait ();

    //udid
    static uint32_t udid        (uint8_t); //0-4 ->UDID1-5

    //misc
    //static uint32_t flash_size  ();
    //static uint32_t ram_size    ();

};

// PIC32MM0064GPM028 0x07708053 0b0000 0111 0111 0000 1000 0000 0101 0011 64/16
// PIC32MM0128GPM028 0x07710053 0b0000 0111 0111 0001 0000 0000 0101 0011 128/16
// PIC32MM0256GPM028 0x07718053 0b0000 0111 0111 0001 1000 0000 0101 0011 256/32

// PIC32MM0064GPM036 0x0770A053 0b0000 0111 0111 0000 1010 0000 0101 0011
// PIC32MM0128GPM036 0x07712053 0b0000 0111 0111 0001 0010 0000 0101 0011
// PIC32MM0256GPM036 0x0771A053 0b0000 0111 0111 0001 1010 0000 0101 0011

// PIC32MM0064GPM048 0x0772C053 0b0000 0111 0111 0010 1100 0000 0101 0011
// PIC32MM0128GPM048 0x07734053 0b0000 0111 0111 0011 0100 0000 0101 0011
// PIC32MM0256GPM048 0x0773C053 0b0000 0111 0111 0011 1100 0000 0101 0011

// PIC32MM0064GPM064 0x0770E053 0b0000 0111 0111 0000 1110 0000 0101 0011
// PIC32MM0128GPM064 0x07716053 0b0000 0111 0111 0001 0110 0000 0101 0011
// PIC32MM0256GPM064 0x0771E053 0b0000 0111 0111 0001 1110 0000 0101 0011
