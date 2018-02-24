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

using vu32ptr = volatile uint32_t*;

//=============================================================================
    uint8_t     Nvm::unlock         ()
//=============================================================================
{
    uint8_t idstat = Sys::unlock_wait();
    Reg::val(NVMKEY, MAGIC1);
    Reg::val(NVMKEY, MAGIC2);
    return idstat;
}

//=============================================================================
    void        Nvm::lock           (uint8_t v)
//=============================================================================
{
    *(vu32ptr)NVMKEY = 0;
    Sys::lock(v);
}

//=============================================================================
    void        Nvm::do_wr          ()
//=============================================================================
{
    uint8_t stat = unlock();
    Reg::setbit(NVMCON, 1<<WR);
    lock(stat);
    while(Reg::anybit(NVMCON, 1<<WR));
}

//=============================================================================
    void        Nvm::do_op          (uint8_t v)
//=============================================================================
{
    Reg::clrbit(NVMCON, NVMOP_CLR | 1<<WREN);
    Reg::setbit(NVMCON, v | 1<<WREN);
    do_wr();
    Reg::clrbit(NVMCON, NVMOP_CLR | 1<<WREN);
}

//=============================================================================
    void        Nvm::address        (uint32_t v)
//=============================================================================
{
    //all addr to physical
    Reg::val(NVMADDR, Reg::k2phys(v));
}

//=============================================================================
    uint8_t     Nvm::write_word     (uint32_t* addr, uint32_t lw)
//=============================================================================
{
    //figure out which word to program, the other will be set to 0xFFFFFFFF
    uint32_t hw = -1;
    if((uint32_t)addr bitand 2){ hw = lw; lw = -1; } //swap hw,lw
    return write_2word(addr, hw, lw);
}

//=============================================================================
    uint8_t     Nvm::write_2word    (uint32_t* addr, uint32_t hw, uint32_t lw)
//=============================================================================
{
    address((uint32_t)addr);
    Reg::val(NVMDATA1, hw);
    Reg::val(NVMDATA0, lw);
    do_op(PGMDWORD);
    return error();
}

//=============================================================================
    uint8_t     Nvm::write_row      (uint32_t* src, uint32_t dst)
//=============================================================================
{
    //flash (dst may be 0 based, OR kseg0 flash addr)
    address(dst | BASEMEM);
    Reg::val(NVMSRCADDR, Reg::k2phys(src)); //sram
    do_op(PGMROW);
    return error();
}

//=============================================================================
    uint8_t     Nvm::page_erase     (uint32_t v)
//=============================================================================
{
    //flash (v may be 0 based, OR kseg0 flash addr)
    address(v | BASEMEM);
    do_op(ERASEPAGE);
    return error();
}

//=============================================================================
    void        Nvm::write_nop      ()
//=============================================================================
{
    do_op(NOP);
}

//=============================================================================
    uint8_t     Nvm::error          ()
//=============================================================================
{
    uint8_t err = (Reg::val16(NVMCON)>>12) bitand 3;
    if(err) write_nop();
    return err;
}

//address | 0x1Dxxxxxx, true=lock until reset
//=============================================================================
    void        Nvm::write_protect  (uint32_t v, bool tf)
//=============================================================================
{
    uint8_t stat = unlock();
    Reg::val(NVMPWP, (v bitand PWP_CLR) | not tf<<PWPULOCK);
    lock(stat);
}

//=============================================================================
    void        Nvm::boot_protect   (BOOTP e, bool tf)
//=============================================================================
{
    uint8_t stat = unlock();
    Reg::val(NVMBWP, e | not tf<<BWPULOCK);
    lock(stat);
}
