#include "UsbBdt.hpp"
#include "Reg.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>

enum BDTCTRLSTAT : uint32_t {
    COUNT_SHIFT = 16, COUNT_MASK = 1023,
    UOWN_SHIFT = 7,
    DATA01_SHIFT = 6,
    KEEP_SHIFT = 5,
    NINC_SHIFT = 4,
    DTS_SHIFT = 3,
    BSTALL_SHIFT = 2,
    PID_SHIFT = 2, PID_MASK = 15
};


UsbBst::bdt_t UsbBdt::table[] = {0};


//=============================================================================
    void        UsbBdt::init            ()
//=============================================================================
{
    table = {0};
}

//n = table offset
//if using U1STAT/stat_endp() , then have to >>2 for n=0-63
//=============================================================================
    void        UsbBdt::bufaddr         (uint8_t n, uint32_t v)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].bufaddr = Reg::k2phys(v);
}

//=============================================================================
    uint32_t    UsbBdt::bufaddr         (uint8_t n)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return 0;
    return Reg::p2kseg1(table[n].bufaddr);
}

//=============================================================================
    void        UsbBdt::count           (uint8_t n, uint16_t v)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (COUNT_MASK<<COUNT_SHIFT);
    table[n].ctrlstat or_eq (v&COUNT_MASK)<<COUNT_SHIFT;
}

//=============================================================================
    uint16_t    UsbBdt::count           (uint8_t n)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return 0;
    return (table[n].ctrlstat >> COUNT_SHIFT) bitand COUNT_MASK;
}

//=============================================================================
    void        UsbBdt::uown            (uint8_t n, bool tf)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (1<<UOWN_SHIFT);
    table[n].ctrlstat or_eq tf<<UOWN_SHIFT;
}

//=============================================================================
    bool        UsbBdt::uown            (uint8_t n)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return false;
    return table[n].ctrlstat bitand (1<<UOWN_SHIFT);
}

//=============================================================================
    void        UsbBdt::data01          (uint8_t n, bool tf)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (1<<DATA01_SHIFT);
    table[n].ctrlstat or_eq tf<<DATA01_SHIFT;
}

//=============================================================================
    bool        UsbBdt::data01          (uint8_t n)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return false;
    return table[n].ctrlstat bitand (1<<DATA01_SHIFT);
}

//=============================================================================
    void        UsbBdt::keep            (uint8_t n, bool tf)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (1<<KEEP_SHIFT);
    table[n].ctrlstat or_eq tf<<KEEP_SHIFT;
}

//=============================================================================
    void        UsbBdt::ninc            (uint8_t n, bool tf)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (1<<NINC_SHIFT);
    table[n].ctrlstat or_eq tf<<NINC_SHIFT;
}

//=============================================================================
    void        UsbBdt::dts             (uint8_t n, bool tf)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (1<<DTS_SHIFT);
    table[n].ctrlstat or_eq tf<<DTS_SHIFT;
}

//=============================================================================
    void        UsbBdt::bstall          (uint8_t n, bool tf)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return;
    table[n].ctrlstat and_eq compl (1<<BSTALL_SHIFT);
    table[n].ctrlstat or_eq tf<<BSTALL_SHIFT;
}

//=============================================================================
    uint8_t     UsbBdt::pid             (uint8_t n)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return 0;
    return (table[n].ctrlstat >> PID_SHIFT) bitand PID_MASK;
}


