#pragma once

/*=============================================================================
 Nvm functions
=============================================================================*/

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

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    static uint8_t  pgm_dword       (uint32_t, uint32_t, uint32_t);
    static uint8_t  pgm_row         (uint32_t, uint32_t);
    static uint8_t  page_erase      (uint32_t);
    static void     nop             (); //public for testing

    static void     pgm_protect     (uint32_t);

    enum BOOTP : uint16_t { PAGE0 = 1<<8, PAGE1 = 1<<9, PAGE2 = 1<<10 };

    static void     boot_protect    (BOOTP);



    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    static Reg r;
    static Irq ir;

    enum : uint32_t {
        NVMCON = 0xBF802930, //WR locked
            WR = 1<<15,
            WREN = 1<<14,
            WRERR = 1<<13,
            LVDERR = 1<<12,
            NVMOP_SHIFT = 0, NVMOP_CLR = 7,
            NOP = 0,
            PGMDWORD = 2,
            PGMROW = 3,
            ERASEPAGE = 4,
            //PGMERASE = 7
        NVMKEY = 0xBF802940,
            MAGIC1 = 0xAA996655,
            MAGIC2 = 0x556699AA,
        NVMADDR = 0xBF802950,
        NVMDATA0 = 0xBF802960,
        NVMDATA1 = 0xBF802970,
        NVMSRCADDR = 0xBF802980,
        NVMPWP = 0xBF802990, //locked
            PWPUNLOCK = 1u<<31,
            PWP_SHIFT = 0, PWP_CLR = 0xFFFFFF,
        NVMBWP = 0xBF8029A0, //locked
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

    using vword_ptr = volatile uint32_t*;
};


/*=============================================================================
 all functions inline
=============================================================================*/
uint8_t Nvm::unlock(){
    uint8_t idstat = ir.all_ison();
    ir.disable_all();
    idstat |= Dma::all_suspend()<<1;
    Dma::all_suspend(true);
    *(vword_ptr)NVMKEY = MAGIC1;
    *(vword_ptr)NVMKEY = MAGIC2;
    return idstat;
}
void Nvm::lock(uint8_t v){
    *(vword_ptr)NVMKEY = 0;
    if(v & 1) ir.enable_all();
    if(v & 2) return;
    Dma::all_suspend(false);
}
void Nvm::do_wr(){
    uint8_t stat = unlock();
    r.setbit(NVMCON, WR);
    lock(stat);
}
void Nvm::do_op(uint8_t v){
    r.clrbit(NVMCON, NVMOP_CLR|WREN);
    r.setbit(NVMCON, v|WREN);
    do_wr();
    r.clrbit(NVMCON, NVMOP_CLR|WREN);
}
void Nvm::address(uint32_t v){
    r.val(NVMADDR, r.k2phys(v));
}
uint8_t Nvm::pgm_dword(uint32_t addr, uint32_t hw, uint32_t lw){
    address(addr);
    r.val(NVMDATA1, hw);
    r.val(NVMDATA0, lw);
    do_op(PGMDWORD);
    return error();
}
uint8_t Nvm::pgm_row(uint32_t src, uint32_t dst){
    address(dst);
    r.val(NVMSRCADDR, r.k2phys(src));
    do_op(PGMROW);
    return error();
}
uint8_t Nvm::page_erase(uint32_t v){
    address(v);
    do_op(ERASEPAGE);
    return error();
}
void Nvm::nop(){
    do_op(NOP);
}
uint8_t Nvm::error(){
    uint8_t err = (r.val16(NVMCON)>>12) & 3;
    if(err) nop();
    return err;
}

void Nvm::pgm_protect(uint32_t v){
    uint8_t stat = unlock();
    r.val(NVMPWP, v & PWP_CLR);
    lock(stat);
}
void Nvm::boot_protect(BOOTP e){
    uint8_t stat = unlock();
    r.val(NVMBWP, e);
    lock(stat);
}