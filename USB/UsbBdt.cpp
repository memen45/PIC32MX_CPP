#include "UsbBdt.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>

//UsbBdt

volatile UsbBdt::bdt_t UsbBdt::table[] = {0};

//=============================================================================
    void        UsbBdt::init     ()
//=============================================================================
{
    for(auto& i : table) *(uint64_t*)&i = 0;
}
