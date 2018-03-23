#include "UsbBdt.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>

//UsbBdt

UsbBdt::bdt_t UsbBdt::table[] = {0};

//=============================================================================
    void        UsbBdt::init     ()
//=============================================================================
{
    table = {0};
}
