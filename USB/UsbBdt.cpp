#include "UsbBdt.hpp"
#include "Reg.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>

UsbBst::bdt_t UsbBdt::table[] = {0};

//n = table offset (0-63)
//=============================================================================
    void        UsbBdt::bufaddr         (uint8_t n, uint8_t* v)
//=============================================================================
{
    table[n].ctrl.bufaddr = Reg::k2phys((uint32_t)v);
}

//=============================================================================
    uint8_t*    UsbBdt::bufaddr         (uint8_t n)
//=============================================================================
{
    return (uint8_t*)Reg::p2kseg1(table[n].ctrl.bufaddr);
}



