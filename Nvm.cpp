#include "Nvm.hpp"
#include "Dma.hpp"

//=============================================================================
    uint8_t     Nvm::unlock         ()
//=============================================================================
{
    uint8_t idstat = ir.all_ison();
    ir.disable_all();
    idstat or_eq Dma::all_suspend()<<1;
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
    r.clrbit(NVMCON, NVMOP_CLR | WREN);
    r.setbit(NVMCON, v | WREN);
    do_wr();
    r.clrbit(NVMCON, NVMOP_CLR | WREN);
}

//=============================================================================
    void        Nvm::address        (uint32_t v)
//=============================================================================
{
    //all addr to physical
    r.val(NVMADDR, r.k2phys(v));
}

//=============================================================================
    uint8_t     Nvm::write_dword    (uint32_t* addr, uint32_t hw, uint32_t lw)
//=============================================================================
{
    address((uint32_t)addr);
    r.val(NVMDATA1, hw);
    r.val(NVMDATA0, lw);
    do_op(PGMDWORD);
    return error();
}

//=============================================================================
    uint8_t     Nvm::write_row      (uint32_t* src, uint32_t dst)
//=============================================================================
{
    //flash (dst may be 0 based, OR kseg0 flash addr)
    address(dst | BASEMEM);
    r.val(NVMSRCADDR, r.k2phys(src)); //sram
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
    uint8_t err = (r.val16(NVMCON)>>12) & 3;
    if(err) write_nop();
    return err;
}

//address | 0x1Dxxxxxx, true=lock until reset
//=============================================================================
    void        Nvm::write_protect  (uint32_t v, bool tf)
//=============================================================================
{
    uint8_t stat = unlock();
    r.val(NVMPWP, (v & PWP_CLR) | not tf<<PWPULOCK_SHIFT);
    lock(stat);
}

//=============================================================================
    void        Nvm::boot_protect   (BOOTP e, bool tf)
//=============================================================================
{
    uint8_t stat = unlock();
    r.val(NVMBWP, e | not tf<<BWPULOCK_SHIFT);
    lock(stat);
}
