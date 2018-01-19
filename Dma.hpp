#pragma once

#include <cstdint>
#include "Reg.hpp"

//Dma 0/1/2/3

struct Dma {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate Dma with dma number
    enum DMAX { DMA0 = 0, DMA1, DMA2, DMA3 };

    constexpr Dma(DMAX);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXCON

    enum CHCHAIN : uint8_t { TOHIGHER = 0, TOLOWER = 1, CHAINOFF = 2 };
    enum CHPRI : uint8_t { PRI0 = 0, PRI1, PRI2, PRI3 };

    bool            busy            ();
    void            chain           (CHCHAIN);
    void            on              (bool);
    void            evt_always      (bool);
    void            auto_en         (bool);
    bool            evt             ();
    void            priority        (CHPRI);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXECON

    enum IRQ : uint8_t { IRQOFF = 255 };

    void            irq_abort       (uint8_t);
    void            irq_start       (uint8_t);
    void            start           ();
    void            abort           ();
    void            abort_match     (bool);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXINT

    enum IRQENF : uint8_t { //byte positions, use reg byte acccess
        SRCDONE = 1<<7, SRCHALF = 1<<6, DSTDONE = 1<<5, DSTHALF = 1<<4,
        BLKDONE = 1<<3, CELLDONE = 1<<2, XFERABORT = 1<<1, ADDRERR = 1<<0
    };

    void            irq             (IRQENF, bool);
    bool            flag            (IRQENF);
    void            flag_clr        (IRQENF);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXSSA

    void            sstart_addr     (uint32_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXDSA

    void            dstart_addr     (uint32_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXSSIZ

    void            ssize           (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXDSIZ

    void            dsize           (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXSPTR

    void            spointer        (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXDPTR

    void            dpointer        (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXCSIZ

    void            csize           (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXCPTR

    uint16_t        cpointer        ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //DCHXDAT

    void            pattern         (uint8_t);


    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //common for all instances

    //DMACON
    static void     all_on          (bool);             //dma on/off
    static void     all_suspend     (bool);             //dma suspend
    static bool     all_suspend     ();                 //dma suspended?
    static bool     any_busy        ();                 //dma busy

    //DMASTAT
    static bool     last_rd         ();                 //last dma was read
                                                        //(else was write)
    static uint8_t  last_ch         ();                 //last dma channel

    //DMAADDR
    static uint32_t last_addr       ();                 //last dma address

    //DCRCCON

    enum CRCBYTO : uint8_t { SAME = 0, REVERSE, SWAPH, SWAPB };
    enum CRCBITO : bool { MSB = 0, LSB };
    enum CRCTYPE : bool { LFSR = 0, IP };

    static void     crc_byto        (CRCBYTO);
    static void     crc_bito        (CRCBITO);
    static void     crc_polyn       (uint8_t);
    static void     crc_on          (bool);
    static void     crc_append      (bool);
    static void     crc_type        (CRCTYPE);
    static void     crc_ch          (uint8_t);

    //DCRCDATA
    static void     crc_seed        (uint32_t);
    static uint32_t crc_data        ();

    //DCRCXOR
    static void     crc_xor         (uint32_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    static Reg r;

    enum {
        DMACON = 0xBF808900,
            ON = 1<<15,
            SUSPEND = 1<<12,
            DMABUSY = 1<<11,
        DMASTAT = 0xBF808910,
            RDWR = 1<<3,
            DMACH_SHIFT = 0, DMACH_CLR = 7,
        DMAADDR = 0xBF808920,
        DCRCCON = 0xBF808930,
            BYTO_SHIFT = 28, BYTO_CLR = 3,
            WBO = 1<<27,
            BITO = 1<<24,
            PLEN_SHIFT = 8, PLEN_CLR = 31,
            CRCEN = 1<<7,
            CRCAPP = 1<<6,
            CRCTYP = 1<<5,
            CRCCH_SHIFT = 0, CRCCH_CLR = 7,
        DCRCDATA = 0xBF808940,
        DCRCXOR = 0xBF808950,
        DCH0CON = 0xBF808960, DCH_SPACING = 0x30, //spacing in words
            CHBUSY = 1<<15,
            CHCHNS = 1<<8,
            CHEN = 1<<7,
            CHAED = 1<<6,
            CHCHN = 1<<5,
            CHAEN = 1<<4,
            CHEDET = 1<<2,
            CHPRI_SHIFT = 0, CHPRI_CLR = 3,
        //register offsets from base in words
        //(m_dmax_con is pointer, so addition will be in words)
        DCHXECON = 4,
            CHAIRQ_SHIFT = 16, CHAIRG_CLR = 255, //can use byte access
            CHSIRQ_SHIFT = 8, CHSIRQ_CLR = 255, //can use byte access
            CFORCE = 1<<7,
            CABORT = 1<<6,
            PATEN = 1<<5,
            SIRQEN = 1<<4,
            AIRQEN = 1<<3,
        DCHXINT = 8,
            CHSDIE = 1<<23,
            CHSHIE = 1<<22,
            CHDDIE = 1<<21,
            CHDHIE = 1<<20,
            CHBCIE = 1<<19,
            CHCCIE = 1<<18,
            CHTAIE = 1<<17,
            CHERIE = 1<<16,
            CHSDIF = 1<<7,
            CHSHIF = 1<<6,
            CHDDIF = 1<<5,
            CHDHIF = 1<<4,
            CHBCIF = 1<<3,
            CHCCIF = 1<<2,
            CHTAIF = 1<<1,
            CHERIF = 1<<0,
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

    volatile uint32_t* m_dmax_con;

    using vbyte_ptr = volatile uint8_t*;            //access reg as bytes

};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//  functions inline

//.............................................................................
    constexpr       Dma::Dma            (DMAX e)
//.............................................................................
    : m_dmax_con((volatile uint32_t*)DCH0CON+(e*DCH_SPACING))
{
}
//common static functions
//DMACON
//.............................................................................
    void            Dma::all_on         (bool tf)
//.............................................................................
{
    r.setbit(DMACON, ON, tf);
}
//.............................................................................
    void            Dma::all_suspend    (bool tf)
//.............................................................................
{
    r.setbit(DMACON, SUSPEND, tf);
}
//.............................................................................
    bool            Dma::all_suspend    ()
//.............................................................................
{
    return r.anybit(DMACON, SUSPEND);
}
//.............................................................................
    bool            Dma::any_busy       ()
//.............................................................................
{
    return r.anybit(DMACON, DMABUSY);
}
//DMASTAT
//.............................................................................
    bool            Dma::last_rd        ()
//.............................................................................
{
    return r.anybit(DMASTAT, RDWR);
}
//.............................................................................
    uint8_t         Dma::last_ch        ()
//.............................................................................
{
    return r.val8(DMASTAT) & DMACH_CLR;
}
//DMAADDR
//.............................................................................
    uint32_t        Dma::last_addr      ()
//.............................................................................
{
    return r.val(DMAADDR);
}
//DCRCCON
//.............................................................................
    void            Dma::crc_byto       (CRCBYTO e)
//.............................................................................
{
    r.clrbit(DCRCCON, BYTO_CLR<<BYTO_SHIFT);
    r.setbit(DCRCCON, e<<BYTO_SHIFT);
    r.setbit(DCRCCON, WBO, (bool)e);
}
//.............................................................................
    void            Dma::crc_bito       (CRCBITO e)
//.............................................................................
{
    r.setbit(DCRCCON, BITO, e);
}
//.............................................................................
    void            Dma::crc_polyn      (uint8_t v)
//.............................................................................
{
    r.clrbit(DCRCCON, PLEN_CLR<<PLEN_SHIFT);
    r.setbit(DCRCCON, (v & PLEN_CLR)<<PLEN_SHIFT);
}
//.............................................................................
    void            Dma::crc_on         (bool tf)
//.............................................................................
{
    r.setbit(DCRCCON, CRCEN, tf);
}
//.............................................................................
    void            Dma::crc_append     (bool tf)
//.............................................................................
{
    r.setbit(DCRCCON, CRCAPP, tf);
}
//.............................................................................
    void            Dma::crc_type       (CRCTYPE e)
//.............................................................................
{
    r.setbit(DCRCCON, CRCTYP, e);
}
//.............................................................................
    void            Dma::crc_ch         (uint8_t v)
//.............................................................................
{
    r.clrbit(DCRCCON, CRCCH_CLR<<CRCCH_SHIFT);
    r.setbit(DCRCCON, (v & CRCCH_CLR)<<CRCCH_SHIFT);
}
//DCRCDATA
//.............................................................................
    void            Dma::crc_seed       (uint32_t v)
//.............................................................................
{
    r.val(DCRCDATA, v);
}
//.............................................................................
uint32_t            Dma::crc_data       ()
//.............................................................................
{
    return r.val(DCRCDATA);
}
//DCRCXOR
//.............................................................................
    void            Dma::crc_xor        (uint32_t v)
//.............................................................................
{
    r.val(DCRCXOR, v);
}
//functions for each channel (class instance)
//DCHXCON
//.............................................................................
    bool            Dma::busy           ()
//.............................................................................
{
    return r.anybit(m_dmax_con, CHBUSY);
}
//.............................................................................
    void            Dma::chain          (CHCHAIN e)
//.............................................................................
{
    r.setbit(m_dmax_con, CHCHNS, e == TOLOWER);
    r.setbit(m_dmax_con, CHCHN, e != CHAINOFF);
}
//.............................................................................
    void            Dma::on             (bool tf)
//.............................................................................
{
    r.setbit(m_dmax_con, CHEN, tf);
    while(busy() != tf);
}
//.............................................................................
    void            Dma::evt_always     (bool tf)
//.............................................................................
{
    r.setbit(m_dmax_con, CHAED, tf);
}
//.............................................................................
    void            Dma::auto_en        (bool tf)
//.............................................................................
{
    r.setbit(m_dmax_con, CHAEN, tf);
}
//.............................................................................
    bool            Dma::evt            ()
//.............................................................................
{
    return r.anybit(m_dmax_con, CHEDET);
}
//.............................................................................
    void            Dma::priority       (CHPRI e)
//.............................................................................
{
    r.clrbit(m_dmax_con, CHPRI_CLR<<CHPRI_SHIFT);
    r.setbit(m_dmax_con, e<<CHPRI_SHIFT);
}
//DCHXECON
//.............................................................................
    void            Dma::irq_abort      (uint8_t v)
//.............................................................................
{
    r.val((vbyte_ptr)m_dmax_con+(DCHXECON*4)+2, v);
    r.setbit(m_dmax_con+DCHXECON, AIRQEN, v != (uint8_t)IRQOFF);
}
//.............................................................................
    void            Dma::irq_start      (uint8_t v)
//.............................................................................
{
    r.val((vbyte_ptr)m_dmax_con+(DCHXECON*4)+1, v);
    r.setbit(m_dmax_con+DCHXECON, SIRQEN, v != (uint8_t)IRQOFF);
}
//.............................................................................
    void            Dma::start          ()
//.............................................................................
{
    r.setbit(m_dmax_con+DCHXECON, CFORCE);
}
//.............................................................................
    void            Dma::abort          ()
//.............................................................................
{
    r.setbit(m_dmax_con+DCHXECON, CABORT);
}
//.............................................................................
    void            Dma::abort_match    (bool tf)
//.............................................................................
{
    r.setbit(m_dmax_con+DCHXECON, PATEN, tf);
}
//DCHXINT
//.............................................................................
    void            Dma::irq            (IRQENF e, bool tf)
//.............................................................................
{
    r.setbit((vbyte_ptr)m_dmax_con+(DCHXINT*4)+2, e, tf);
}
//.............................................................................
    bool            Dma::flag           (IRQENF e)
//.............................................................................
{
    return r.anybit((vbyte_ptr)m_dmax_con+(DCHXINT*4)+2, e);
}
//.............................................................................
    void            Dma::flag_clr       (IRQENF e)
//.............................................................................
{
    r.clrbit((vbyte_ptr)m_dmax_con+(DCHXINT*4), e);
}
//DCHXSSA
//.............................................................................
    void            Dma::sstart_addr    (uint32_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXSSA, r.k2phys(v));
}
//DCHXDSA
//.............................................................................
    void            Dma::dstart_addr    (uint32_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXDSA, r.k2phys(v));
}
//DCHXSSIZ
//.............................................................................
    void            Dma::ssize          (uint16_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXSSIZ, v);
}
//DCHXDSIZ
//.............................................................................
    void            Dma::dsize          (uint16_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXDSIZ, v);
}
//DCHXSPTR
//.............................................................................
    void            Dma::spointer       (uint16_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXSPTR, v);
}
//DCHXDPTR
//.............................................................................
    void            Dma::dpointer       (uint16_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXDPTR, v);
}
//DCHXCSIZ
//.............................................................................
    void            Dma::csize          (uint16_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXCSIZ, v);
}
//DCHXCPTR
//.............................................................................
    uint16_t        Dma::cpointer       ()
//.............................................................................
{
    return r.val16(m_dmax_con+DCHXCPTR);
}
//DCHXDAT
//.............................................................................
    void            Dma::pattern        (uint8_t v)
//.............................................................................
{
    r.val(m_dmax_con+DCHXDAT, v);
}