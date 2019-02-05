#include "Nvm.hpp"
#include "Dma.hpp"
#include "Irq.hpp"
#include "Sys.hpp"

enum : uint32_t {
NVMCON = 0xBF80F400, //WR locked
    WR = 15,
    WREN = 14,
    WRERR = 13,
    LVDERR = 12,
	LVDSTAT = 11,
    NVMOP_SHIFT = 0, NVMOP_CLR = 15,
    NOP = 0,
	PGMWORD = 1,
    PGMROW = 3,
    ERASEPAGE = 4,
    //PGMERASE = 5
NVMKEY = 0xBF80F410,
    MAGIC1 = 0xAA996655,
    MAGIC2 = 0x556699AA,
NVMADDR = 0xBF80F420, //physical address
NVMDATA = 0xBF80F430,
NVMSRCADDR = 0xBF80F440, //physical address

};


//-----------------------------------------------------------------private-----
            static auto
unlock      () -> void
            {
            //use Sys:: function to prevent irq/dma interruption of these
            //two writes (although a sys unlock not needed)
            uint8_t idstat = Sys::unlock_wait();
            Reg::val(NVMKEY, MAGIC1);
            Reg::val(NVMKEY, MAGIC2);
            Sys::lock(idstat);
            }

//-----------------------------------------------------------------private-----
            static auto
lock        () -> void
            {
            Reg::val(NVMKEY, 0);
            }

//-----------------------------------------------------------------private-----
            static auto
do_wr       () -> void
            {
            unlock();
            Reg::setbit(NVMCON, 1<<WR);
            lock();
            while(Reg::anybit(NVMCON, 1<<WR));
            }

//-----------------------------------------------------------------private-----
            static auto
do_op       (uint8_t v) -> void
            {
            Reg::clrbit(NVMCON, NVMOP_CLR bitor (1<<WREN));
            Reg::setbit(NVMCON, v bitor (1<<WREN));
            do_wr();
            Reg::clrbit(NVMCON, NVMOP_CLR bitor (1<<WREN));
            }

//-----------------------------------------------------------------private-----
            static auto
address     (uint32_t v) -> void
            {
            //0 based values to kseg0
            v or_eq Nvm::BASEFLASH;
            //kseg0 values to physical
            Reg::val(NVMADDR, Reg::k2phys(v));
            }

//-----------------------------------------------------------------private-----
            static auto
error       () -> uint8_t
            {
            uint8_t err = (Reg::val16(NVMCON)>>12) bitand 3;
            if(err) do_op(NOP);
            return err;
            }

//=============================================================================
            auto Nvm::
mem_size    () -> uint32_t
            {
            //32K << 1,2,3 = 64k,128k,256k
            return Sys::flash_size();
            }

//=============================================================================
            auto Nvm::
write_word (uint32_t addr, uint32_t w) -> uint8_t
            {
            address(addr);
            val(NVMDATA, w);
            do_op(PGMWORD);
            return error();
            }

//=============================================================================
            auto Nvm::
write_row   (uint32_t src, uint32_t dst) -> uint8_t
            {
            //flash (dst may be 0 based, OR kseg0 flash addr)
            address(dst);
            val(NVMSRCADDR, k2phys(src)); //sram
            do_op(PGMROW);
            return error();
            }

//=============================================================================
            auto Nvm::
page_erase  (uint32_t v) -> uint8_t
            {
            //flash (v may be 0 based, OR kseg0 flash addr)
            address(v);
            do_op(ERASEPAGE);
            return error();
            }
