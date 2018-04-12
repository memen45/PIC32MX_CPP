#include "UsbBdt.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include <cstring> //memset

//UsbBdt

volatile UsbBdt::bdt_t UsbBdt::table[][2][2] = {0};

//=============================================================================
    void        UsbBdt::init     ()
//=============================================================================
{
    memset((void*)table, 0, sizeof(table));
}
