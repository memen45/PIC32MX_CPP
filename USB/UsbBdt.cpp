#include "UsbBdt.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>


Usb::BDT_ENTRY UsbBdt::table[16] = {0};

//=============================================================================
    void        UsbBdt::table_clr           ()
//=============================================================================
{
    table = {0};
}
