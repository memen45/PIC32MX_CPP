#pragma once

#include <cstdint>
#include "Reg.hpp"
#include "Osc.hpp"

/*=============================================================================
 MCCP 1-9 functions
=============================================================================*/

struct Ccp  {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate Ccp with ccp number
    enum CCPX {
        CCPX1 = 0, CCPX2, CCPX3, CCPX4, CCPX5, CCPX6, CCPX7, CCPX8, CCPX9
    };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    constexpr Ccp(CCPX);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxcon1

    enum OPOSTSRC : bool { TMRIRQ = 0, EVTTRG };
    enum SYNC : uint8_t {
        TMR = 0, ME, MCCP1, SCCP2, SCCP3, INT0 = 9, INT1, INT2, INT3, INT4,
        SCCP8, SCCP9, CLC1, CLC2, CLC3, CLC4, COMP1 = 24, COMP2, COMP3, ADC,
        OFF = 31
    };
    enum CLKSEL : uint8_t {
        SYSCLK = 0, REFO1, SOSC,
        CLC1MCCP1 = 3, CLC2MCCP2 = 3, CLC3MCCP3 = 3, CLC1SCCP4 = 3,
        CLC2SCCP5 = 3, CLC3SCCP6 = 3, CLC4SCCP7 = 3, CLC1SCCP8 = 3,
        CLC1SCCP9 = 3, TCKIB = 6, TCKIA = 7
    };
    enum TMRPS : uint8_t { PS1 = 0, PS4, PS16, PS64 };
    enum MODE : uint8_t {
        TMRONLY = 0, HIMATCH, LOMATCH, TOGMATCH, EDGE, EDGEBUF,
        PULSEBUF, VFPULSE, EXTIN = 15,
        //bit4=1=CAPTURE MODE = CCSEL=1
        CAPEDGE = 16, CAPRISE, CAPFALL, CAPRISEFALL, CAP4RISE, CAP16RISE
    };

    void            outscale_src    (OPOSTSRC);
    void            retrigger       (bool);
    void            outscale        (uint8_t);
    void            trigger         (bool);
    void            oneshot         (bool);
    void            altsync         (bool);
    void            sync_src        (SYNC);
    void            on              (bool);
    void            stop_idle       (bool);
    void            stop_sleep      (bool);
    void            tmr_sync        (bool);
    void            clk_src         (CLKSEL);
    void            tmr_prescale    (TMRPS);
    void            tmr32           (bool);
    void            mode            (MODE);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxcon2

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxcon3

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxstat

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxtmr

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxpr

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxra

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxrb

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //ccpxbuf




    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    Reg r;

    enum : uint32_t {
        CCP1CON1 = 0xBF800100, CCPX_SPACING = 0x40,  //spacing in words
            OPSSRC = 1u<<31,
            RTRGEN = 1<<30,
            OPS_SHIFT = 24, OPS_CLR = 15,
            TRIGEN = 1<<23,
            ONESHOT = 1<<22,
            ALTSYNC = 1<<21,
            SYNC_SHIFT = 16, SYNC_CLR = 31,
            ON = 1<<15,
            SIDL = 1<<13,
            CCPSLP = 1<<12,
            TMRSYNC = 1<<11,
            CLKSEL_SHIFT = 1, CLKSEL_CLR = 7,
            TMRPS_SHIFT = 6, TMRPS_CLR = 3,
            T32 = 1<<5,
            CCSEL = 1<<4,
            MOD_SHIFT = 0, MOD_CLR = 15,
        CCPXCON2 = 4, //offset in words from ccp1con1
            OENSYSNC = 1u<<31,
            OCFEN = 1<<29,
            OCEEN = 1<<28,
            OCDEN = 1<<27,
            OCCEN = 1<<26,
            OCBEN = 1<<25,
            OCAEN = 1<<24,
            ICGSM_SHIFT = 22, ICGSM_CLR = 3,
            AUXOUT_SHIFT = 19, AUXOUT_CLR = 3,
            ICS_SHIFT = 16, ICS_CLR = 7,
            PWMRSEN = 1<<15,
            ASDGM = 1<<14,
            SSDG = 1<<12,
            ASDG_SHIFT = 0, ASDG_CLR = 255,
        CCPXCON3 = 8,
            OETRIG = 1u<<31,
            OSCNT_SHIFT = 28, OSCNT_CLR = 7,
            OUTM_SHIFT = 24, OUTM_CLR = 7,
            POLACE = 1<<21,
            POLBDF = 1<<20,
            PSSACE_SHIFT = 18, PSSACE_CLR = 3,
            PSSBDF_SHIFT = 16, PSSBDF_CLR = 3,
            DT_SHIFT = 0, DT_CLR = 63,
        CCPXSTAT = 12,
            PRLWIP = 1<<20,
            TMRHWIP = 1<<19,
            TMRLWIP = 1<<18,
            RBWIP = 1<<17,
            RAWIP = 1<<16,
            ICGARM = 1<<10,
            CCPTRIG = 1<<7,
            TRSET = 1<<6,
            TRCLR = 1<<5,
            ASEVT = 1<<4,
            SCEVT = 1<<3,
            ICDIS = 1<<2,
            ICOV = 1<<1,
            ICBNE = 1<<0,
        CCPXTMR = 16,
        CCPXPR = 20,
        CCPXRA = 24,
        CCPXRB = 28,
        CCPXBUF = 32,
    };

    using vbyte_ptr = volatile uint8_t*;
    using vword_ptr = volatile uint32_t*;

    vword_ptr m_ccpx_con;
};


/*=============================================================================
 inline functions
=============================================================================*/
constexpr Ccp::Ccp(CCPX e)
    : m_ccpx_con((vword_ptr)CCP1CON1+(e*CCPX_SPACING))
{}

//ccpxcon1
void Ccp::outscale_src(OPOSTSRC e){
    r.setbit(m_ccpx_con, OPSSRC, e);
}
void Ccp::retrigger(bool tf){
    r.setbit(m_ccpx_con, RTRGEN, tf);
}
void Ccp::outscale(uint8_t v){
    r.clrbit(m_ccpx_con, OPS_CLR<<OPS_SHIFT);
    r.setbit(m_ccpx_con, v<<OPS_SHIFT);
}
void Ccp::trigger(bool tf){
    r.setbit(m_ccpx_con, TRIGEN, tf);
}
void Ccp::oneshot(bool tf){
    r.setbit(m_ccpx_con, ONESHOT, tf);
}
void Ccp::altsync(bool tf){
    r.setbit(m_ccpx_con, ALTSYNC, tf);
}
void Ccp::sync_src(SYNC e){
    r.clrbit(m_ccpx_con, SYNC_CLR<<SYNC_SHIFT);
    r.setbit(m_ccpx_con, e<<SYNC_SHIFT);
}
void Ccp::on(bool tf){
    r.setbit(m_ccpx_con, ON, tf);
}
void Ccp::stop_idle(bool tf){
    r.setbit(m_ccpx_con, SIDL, !tf);
}
void Ccp::stop_sleep(bool tf){
    r.setbit(m_ccpx_con, CCPSLP, !tf);
}
void Ccp::tmr_sync(bool tf){
    r.setbit(m_ccpx_con, TMRSYNC, tf);
}
void Ccp::clk_src(CLKSEL e){
    r.clrbit(m_ccpx_con, CLKSEL_CLR<<CLKSEL_SHIFT);
    r.setbit(m_ccpx_con, e<<CLKSEL_SHIFT);
}
void Ccp::tmr_prescale(TMRPS e){
    r.clrbit(m_ccpx_con, TMRPS_CLR<<TMRPS_SHIFT);
    r.setbit(m_ccpx_con, e<<TMRPS_SHIFT);
}
void Ccp::tmr32(bool tf){
    r.setbit(m_ccpx_con, T32, tf);
}
void Ccp::mode(MODE e){
    r.setbit(m_ccpx_con, CCSEL, e & (1<<4));
    r.clrbit(m_ccpx_con, MOD_CLR<<MOD_SHIFT);
    r.setbit(m_ccpx_con, e<<MOD_SHIFT);
}
