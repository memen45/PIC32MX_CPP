#include "Nvm.hpp"
#include "Dma.hpp"
#include "Reg.hpp"
#include "Irq.hpp"
#include "Sys.hpp"

enum : uint32_t {
    NVMCON = 0xBF802930, //WR locked
        WR = 15,
        WREN = 14,
        WRERR = 13,
        LVDERR = 12,
        NVMOP_SHIFT = 0, NVMOP_CLR = 15,
        NOP = 0,
        PGMDWORD = 2,
        PGMROW = 3,
        ERASEPAGE = 4,
        //PGMERASE = 7
    NVMKEY = 0xBF802940,
        MAGIC1 = 0xAA996655,
        MAGIC2 = 0x556699AA,
    NVMADDR = 0xBF802950, //physical address
    NVMDATA0 = 0xBF802960,
    NVMDATA1 = 0xBF802970,
    NVMSRCADDR = 0xBF802980, //physical address
    NVMPWP = 0xBF802990, //locked
        PWPULOCK = 31,
        PWP_SHIFT = 0, PWP_CLR = 0xFFFFFF,
    NVMBWP = 0xBF8029A0, //locked
        BWPULOCK = 15,
        BWP2 = 10,
        BWP1 = 9,
        BWP0 = 8
};

//-----------------------------------------------------------------private-----
    static auto
    unlock () -> uint8_t
//-----------------------------------------------------------------------------
{
    uint8_t idstat = Sys::unlock_wait();
    Reg::val(NVMKEY, MAGIC1);
    Reg::val(NVMKEY, MAGIC2);
    return idstat;
}

//-----------------------------------------------------------------private-----
    static auto
    lock (uint8_t v) -> void
//-----------------------------------------------------------------------------
{
    Reg::val(NVMKEY, 0);
    Sys::lock(v);
}

//-----------------------------------------------------------------private-----
    static auto
    do_wr () -> void
//-----------------------------------------------------------------------------
{
    uint8_t stat = unlock();
    Reg::setbit(NVMCON, 1<<WR);
    lock(stat);
    while(Reg::anybit(NVMCON, 1<<WR));
}

//-----------------------------------------------------------------private-----
    static auto
    do_op (uint8_t v) -> void
//-----------------------------------------------------------------------------
{
    Reg::clrbit(NVMCON, NVMOP_CLR bitor (1<<WREN));
    Reg::setbit(NVMCON, v bitor (1<<WREN));
    do_wr();
    Reg::clrbit(NVMCON, NVMOP_CLR bitor (1<<WREN));
}

//-----------------------------------------------------------------private-----
    static auto
    address (uint32_t v) -> void
//-----------------------------------------------------------------------------
{
    //all addr to physical (and 0 based to kseg based)
    v or_eq Nvm::BASEMEM;
    Reg::val(NVMADDR, Reg::k2phys(v));
}

//-----------------------------------------------------------------private-----
    static auto
    error () -> uint8_t
//-----------------------------------------------------------------------------
{
    uint8_t err = (Reg::val16(NVMCON)>>12) bitand 3;
    if(err) do_op(NOP);
    return err;
}

//=============================================================================
    auto Nvm::
    write_2word (uint32_t addr, uint32_t hw, uint32_t lw) -> uint8_t
//=============================================================================
{
    address(addr);
    Reg::val(NVMDATA1, hw);
    Reg::val(NVMDATA0, lw);
    do_op(PGMDWORD);
    return error();
}

//=============================================================================
    auto Nvm::
    write_row (uint32_t src, uint32_t dst) -> uint8_t
//=============================================================================
{
    //flash (dst may be 0 based, OR kseg0 flash addr)
    address(dst);
    Reg::val(NVMSRCADDR, Reg::k2phys(src)); //sram
    do_op(PGMROW);
    return error();
}

//=============================================================================
    auto Nvm::
    page_erase (uint32_t v) -> uint8_t
//=============================================================================
{
    //flash (v may be 0 based, OR kseg0 flash addr)
    address(v);
    do_op(ERASEPAGE);
    return error();
}

//address | 0x1Dxxxxxx, true=lock until reset
//=============================================================================
    auto Nvm::
    write_protect (uint32_t v, bool tf) -> void
//=============================================================================
{
    uint8_t stat = unlock();
    Reg::val(NVMPWP, (v bitand PWP_CLR) | not tf<<PWPULOCK);
    lock(stat);
}

//=============================================================================
    auto Nvm::
    boot_protect (BOOTP e, bool tf) -> void
//=============================================================================
{
    uint8_t stat = unlock();
    Reg::val(NVMBWP, e | not tf<<BWPULOCK);
    lock(stat);
}
