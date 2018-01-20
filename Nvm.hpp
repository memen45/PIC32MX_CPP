#pragma once

//Nvm

#include <cstdint>
#include "Reg.hpp"
#include "Irq.hpp"
#include "Dma.hpp"

/*
    all done from flash, so check of wr for completion is not made
    (cpu is halted until done, no need)

    return values
    0= no error, 1= low voltage error, 2= write error
*/

struct Nvm {

    enum MEM : uint32_t {
        BASEMEM = 0x9D000000,
        MAXMEM64 = 0xFFFF,
        MAXMEM128 = 0x1FFFF,
        MAXMEM256 = 0x3FFFF,
        PAGESIZE = 512*4, //512words, 2048bytes
        ROWSIZE = 64*4, //64words, 256bytes
        MAXPAGE256 = ((MAXMEM256+1)/PAGESIZE-1), //0-127
        MAXPAGE128 = ((MAXMEM128+1)/PAGESIZE-1), //0-63
        MAXPAGE64 = ((MAXMEM64+1)/PAGESIZE-1), //0-31
        MAXROW256 = ((MAXMEM256+1)/ROWSIZE-1), //0-1023
        MAXROW128 = ((MAXMEM128+1)/ROWSIZE-1), //0-511
        MAXROW64 = ((MAXMEM64+1)/ROWSIZE-1), //0-255
    };

    enum BOOTP : uint16_t { PAGE0 = 1<<8, PAGE1 = 1<<9, PAGE2 = 1<<10 };

    static uint8_t  pgm_dword       (uint32_t*, uint32_t, uint32_t);
    static uint8_t  pgm_row         (uint32_t*, uint32_t); //src,dst
    static uint8_t  page_erase      (uint32_t);
    static void     pgm_protect     (uint32_t, bool); //true=lock until reset
    static void     boot_protect    (BOOTP, bool); //true=lock until reset

    private:

    static Reg r;
    static Irq ir;

    enum : uint32_t {
        NVMCON = 0xBF802930, //WR locked
            WR = 1<<15,
            WREN = 1<<14,
            WRERR = 1<<13,
            LVDERR = 1<<12,
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
            PWPULOCK = 1u<<31,
            PWPULOCK_SHIFT = 31,
            PWP_SHIFT = 0, PWP_CLR = 0xFFFFFF,
        NVMBWP = 0xBF8029A0, //locked
            BWPULOCK_SHIFT = 15,
            BWPULOCK = 1<<15,
            BWP2 = 1<<10,
            BWP1 = 1<<9,
            BWP0 = 1<<8
    };

    static uint8_t  unlock      ();
    static void     lock        (uint8_t);
    static void     do_wr       ();
    static void     do_op       (uint8_t);
    static void     address     (uint32_t);
    static uint8_t  error       ();
    static void     pgm_nop     ();

    using vword_ptr = volatile uint32_t*;
};

//=============================================================================
    uint8_t     Nvm::unlock         ()
//=============================================================================
{
    uint8_t idstat = ir.all_ison();
    ir.disable_all();
    idstat |= Dma::all_suspend()<<1;
    Dma::all_suspend(true);
    r.val(NVMKEY, MAGIC1);
    r.val(NVMKEY, MAGIC2);
    return idstat;
}

//=============================================================================
    void        Nvm::lock           (uint8_t v)
//=============================================================================
{
    *(vword_ptr)NVMKEY = 0;
    if(v & 1) ir.enable_all();
    if(v & 2) return;
    Dma::all_suspend(false);
}

//=============================================================================
    void        Nvm::do_wr          ()
//=============================================================================
{
    uint8_t stat = unlock();
    r.setbit(NVMCON, WR);
    lock(stat);
    while(r.anybit(NVMCON, WR));
}

//=============================================================================
    void        Nvm::do_op          (uint8_t v)
//=============================================================================
{
    r.clrbit(NVMCON, NVMOP_CLR|WREN);
    r.setbit(NVMCON, v|WREN);
    do_wr();
    r.clrbit(NVMCON, NVMOP_CLR|WREN);
}

//=============================================================================
    void        Nvm::address        (uint32_t v)
//=============================================================================
{
    //all addr to physical
    r.val(NVMADDR, r.k2phys(v));
}

//=============================================================================
    uint8_t     Nvm::pgm_dword      (uint32_t* addr, uint32_t hw, uint32_t lw)
//=============================================================================
{
    address((uint32_t)addr);
    r.val(NVMDATA1, hw);
    r.val(NVMDATA0, lw);
    do_op(PGMDWORD);
    return error();
}

//=============================================================================
    uint8_t     Nvm::pgm_row        (uint32_t* src, uint32_t dst)
//=============================================================================
{
    //flash (dst may be 0 based, OR kseg0 flash addr)
    address(dst|BASEMEM);
    r.val(NVMSRCADDR, r.k2phys(src)); //sram
    do_op(PGMROW);
    return error();
}

//=============================================================================
    uint8_t     Nvm::page_erase     (uint32_t v)
//=============================================================================
{
    //flash (v may be 0 based, OR kseg0 flash addr)
    address(v|BASEMEM);
    do_op(ERASEPAGE);
    return error();
}

//=============================================================================
    void        Nvm::pgm_nop        ()
//=============================================================================
{
    do_op(NOP);
}

//=============================================================================
    uint8_t     Nvm::error          ()
//=============================================================================
{
    uint8_t err = (r.val16(NVMCON)>>12) & 3;
    if(err) pgm_nop();
    return err;
}

//address | 0x1Dxxxxxx, true=lock until reset
//=============================================================================
    void        Nvm::pgm_protect    (uint32_t v, bool tf)
//=============================================================================
{
    uint8_t stat = unlock();
    r.val(NVMPWP, (v & PWP_CLR) | !tf<<PWPULOCK_SHIFT);
    lock(stat);
}

//=============================================================================
    void        Nvm::boot_protect   (BOOTP e, bool tf)
//=============================================================================
{
    uint8_t stat = unlock();
    r.val(NVMBWP, e | !tf<<BWPULOCK_SHIFT);
    lock(stat);
}