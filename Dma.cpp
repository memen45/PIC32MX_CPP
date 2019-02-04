#include "Dma.hpp"

enum {
DMACON = 0xBF808900,
    ON = 15,
    SUSPEND = 12,
    DMABUSY = 11,
DMASTAT = 0xBF808910,
    RDWR = 3,
    DMACH_SHIFT = 0, DMACH_CLR = 7,
DMAADDR = 0xBF808920,
DCRCCON = 0xBF808930,
    BYTO_SHIFT = 28, BYTO_CLR = 3,
    WBO = 27,
    BITO = 24,
    PLEN_SHIFT = 8, PLEN_CLR = 31,
    CRCEN = 7,
    CRCAPP = 6,
    CRCTYP = 5,
    CRCCH_SHIFT = 0, CRCCH_CLR = 7,
DCRCDATA = 0xBF808940,
DCRCXOR = 0xBF808950,

DCH_SPACING = 0x30, //spacing in words
DCH0CON = 0xBF808960,
    CHBUSY = 15,
    CHCHNS = 8,
    CHEN = 7,
    CHAED = 6,
    CHCHN = 5,
    CHAEN = 4,
    CHEDET = 2,
    CHPRI_SHIFT = 0, CHPRI_CLR = 3,
//register offsets from base in words
//(m_dmax_con is pointer, so addition will be in words)
DCHXECON = 4,
    CHAIRQ_SHIFT = 16, CHAIRG_CLR = 255, //can use byte access
    CHSIRQ_SHIFT = 8, CHSIRQ_CLR = 255, //can use byte access
    CFORCE = 7,
    CABORT = 6,
    PATEN = 5,
    SIRQEN = 4,
    AIRQEN = 3,
DCHXINT = 8,
DCHXSSA = 12,
DCHXDSA = 16,
DCHXSSIZ = 20,
DCHXDSIZ = 24,
DCHXSPTR = 28,
DCHXDPTR = 32,
DCHXCSIZ = 36,
DCHXCPTR = 40,
DCHXDAT = 44
};


//=============================================================================
            Dma::
Dma         (DMAX e)
            : m_dmax_con((volatile uint32_t*)DCH0CON + (e * DCH_SPACING))
            {
            }

//common static functions
//DMACON
//=============================================================================
            auto Dma::
all_on      (bool tf) -> void
            {
            setbit(DMACON, 1<<ON, tf);
            }

//=============================================================================
            auto Dma::
all_suspend (bool tf) -> void
            {
            setbit(DMACON, 1<<SUSPEND, tf);
            }

//=============================================================================
            auto Dma::
all_suspend () -> bool
            {
            return anybit(DMACON, 1<<SUSPEND);
            }

//=============================================================================
            auto Dma::
any_busy    () -> bool
            {
            return anybit(DMACON, 1<<DMABUSY);
            }

//DMASTAT
//=============================================================================
            auto Dma::
    last_rd () -> bool
            {
            return anybit(DMASTAT, 1<<RDWR);
            }

//=============================================================================
            auto Dma::
last_ch     () -> uint8_t
            {
            return val8(DMASTAT) bitand DMACH_CLR;
            }

//DMAADDR
//=============================================================================
            auto Dma::
last_addr   () -> uint32_t
            {
            return val(DMAADDR);
            }

//DCRCCON
//=============================================================================
            auto Dma::
crc_byto    (CRCBYTO e) -> void
            {
            clrbit(DCRCCON, BYTO_CLR<<BYTO_SHIFT);
            setbit(DCRCCON, e<<BYTO_SHIFT);
            setbit(DCRCCON, 1<<WBO, (bool)e);
            }

//=============================================================================
            auto Dma::
crc_bito    (CRCBITO e) -> void
            {
            setbit(DCRCCON, 1<<BITO, e);
            }

//=============================================================================
            auto Dma::
crc_polyn   (uint8_t v) -> void
            {
            clrbit(DCRCCON, PLEN_CLR<<PLEN_SHIFT);
            setbit(DCRCCON, (v bitand PLEN_CLR)<<PLEN_SHIFT);
            }

//=============================================================================
            auto Dma::
crc_on      (bool tf) -> void
            {
            setbit(DCRCCON, 1<<CRCEN, tf);
            }

//=============================================================================
            auto Dma::
crc_append  (bool tf) -> void
            {
            setbit(DCRCCON, 1<<CRCAPP, tf);
            }

//=============================================================================
            auto Dma::
crc_type    (CRCTYPE e) -> void
            {
            setbit(DCRCCON, 1<<CRCTYP, e);
            }

//=============================================================================
            auto Dma::
crc_ch      (uint8_t v) -> void
            {
            clrbit(DCRCCON, CRCCH_CLR<<CRCCH_SHIFT);
            setbit(DCRCCON, (v bitand CRCCH_CLR)<<CRCCH_SHIFT);
            }

//DCRCDATA
//=============================================================================
            auto Dma::
crc_seed    (uint32_t v) -> void
            {
            val(DCRCDATA, v);
            }

//=============================================================================
            auto Dma::
crc_data    () -> uint32_t
            {
            return val(DCRCDATA);
            }

//DCRCXOR
//=============================================================================
            auto Dma::
crc_xor     (uint32_t v) -> void
            {
            val(DCRCXOR, v);
            }

//functions for each channel (class instance)
//DCHXCON
//=============================================================================
            auto Dma::
busy        () -> bool
            {
            return anybit(m_dmax_con, 1<<CHBUSY);
            }

//=============================================================================
            auto Dma::
chain       (CHCHAIN e) -> void
            {
            setbit(m_dmax_con, 1<<CHCHNS, e == TOLOWER);
            setbit(m_dmax_con, 1<<CHCHN, e not_eq CHAINOFF);
            }

//=============================================================================
            auto Dma::
on          (bool tf) -> void
            {
            setbit(m_dmax_con, 1<<CHEN, tf);
            while(busy() not_eq tf);
            }

//=============================================================================
            auto Dma::
evt_always  (bool tf) -> void
            {
            setbit(m_dmax_con, 1<<CHAED, tf);
            }

//=============================================================================
            auto Dma::
auto_en     (bool tf) -> void
            {
            setbit(m_dmax_con, 1<<CHAEN, tf);
            }

//=============================================================================
            auto Dma::
evt         () -> bool
            {
            return anybit(m_dmax_con, 1<<CHEDET);
            }

//=============================================================================
            auto Dma::
priority    (CHPRI e) -> void
            {
            clrbit(m_dmax_con, CHPRI_CLR<<CHPRI_SHIFT);
            setbit(m_dmax_con, e<<CHPRI_SHIFT);
            }

//DCHXECON
//=============================================================================
            auto Dma::
irq_abort   (uint8_t v) -> void
            {
            val((vu8ptr)m_dmax_con + (DCHXECON * 4) + 2, v);
            setbit(m_dmax_con + DCHXECON, 1<<AIRQEN,
                        v not_eq (uint8_t)IRQOFF);
            }

//=============================================================================
            auto Dma::
irq_start   (uint8_t v) -> void
            {
            val((vu8ptr)m_dmax_con + (DCHXECON * 4) + 1, v);
            setbit(m_dmax_con + DCHXECON,
                        1<<SIRQEN, v not_eq (uint8_t)IRQOFF);
            }

//=============================================================================
            auto Dma::
start       () -> void
            {
            setbit(m_dmax_con+DCHXECON, 1<<CFORCE);
            }

//=============================================================================
            auto Dma::
abort       () -> void
            {
            setbit(m_dmax_con+DCHXECON, 1<<CABORT);
            }

//=============================================================================
            auto Dma::
abort_match (bool tf) -> void
            {
            setbit(m_dmax_con+DCHXECON, 1<<PATEN, tf);
            }

//DCHXINT
//=============================================================================
            auto Dma::
irq         (IRQENF e, bool tf) -> void
            {
            setbit((vu8ptr)m_dmax_con + (DCHXINT * 4) + 2, e, tf);
            }

//=============================================================================
            auto Dma::
flag        (IRQENF e) -> bool
            {
            return anybit((vu8ptr)m_dmax_con + (DCHXINT * 4) + 2, e);
            }

//=============================================================================
            auto Dma::
flag_clr    (IRQENF e) -> void
            {
            clrbit((vu8ptr)m_dmax_con + (DCHXINT * 4), e);
            }

//DCHXSSA
//=============================================================================
            auto Dma::
sstart_addr (uint32_t v) -> void
            {
            val(m_dmax_con + DCHXSSA, k2phys(v));
            }

//DCHXDSA
//=============================================================================
            auto Dma::
dstart_addr (uint32_t v) -> void
            {
            val(m_dmax_con + DCHXDSA, k2phys(v));
            }

//DCHXSSIZ
//=============================================================================
            auto Dma::
ssize       (uint16_t v) -> void
            {
            val(m_dmax_con + DCHXSSIZ, v);
            }

//DCHXDSIZ
//=============================================================================
            auto Dma::
dsize       (uint16_t v) -> void
            {
            val(m_dmax_con + DCHXDSIZ, v);
            }

//DCHXSPTR
//=============================================================================
            auto Dma::
spointer    (uint16_t v) -> void
            {
            val(m_dmax_con + DCHXSPTR, v);
            }

//DCHXDPTR
//=============================================================================
            auto Dma::
dpointer    (uint16_t v) -> void
            {
            val(m_dmax_con + DCHXDPTR, v);
            }

//DCHXCSIZ
//=============================================================================
            auto Dma::
csize       (uint16_t v) -> void
            {
            val(m_dmax_con + DCHXCSIZ, v);
            }

//DCHXCPTR
//=============================================================================
            auto Dma::
cpointer    () -> uint16_t
            {
            return val16(m_dmax_con + DCHXCPTR);
            }

//DCHXDAT
//=============================================================================
            auto Dma::
pattern     (uint8_t v) -> void
            {
            val(m_dmax_con + DCHXDAT, v);
            }
