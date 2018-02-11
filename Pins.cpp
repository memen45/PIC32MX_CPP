#include "Pins.hpp"
#include "Adc.hpp"
#include "Reg.hpp"
#include "Sys.hpp"

enum { //offsets from base address, in words
    TRIS = 0x10>>2, PORT = 0x20>>2, LAT = 0x30>>2, ODC = 0x40>>2,
    CNPU = 0x50>>2, CNPD = 0x60>>2, CNCON = 0x70>>2, CNEN0 = 0x80>>2,
    CNSTAT = 0x90>>2, CNEN1 = 0xA0>>2, CNF = 0xB0>>2
};

enum {
    ANSELA = 0xBF802BB0, ANSELX_SPACING = 64, //spacing in words
    //CNCONx
        ON = 1<<15,
        CNSTYLE = 1<<11,
    RPCON = 0xBF802A00,
        IOLOCK = 1<<11,
    RPINR1 = 0xBF802A10,
    RPOR0 = 0xBF802B10
};

//IOMODE
// bit  | 4  | 3  |  2   |  1   |  0  |
//      | PD | PU | ACTL | DOUT | DIN |
//-----------------------------------------
//AIN   |    |    |      |      |     | 0
//IN    |    |    |      |      |  1  | 1
//INPU  |    |  1 |  1   |      |  1  | 13
//INPD  |  1 |    |      |      |  1  | 17
//INL   |    |    |  1   |      |  1  | 5
//OUT   |    |    |      |   1  |     | 2
//OUTL  |    |    |  1   |   1  |     | 6

enum : uint8_t { ACTL = 1<<2  }; //IOMODE ACTL bit (active low bit)

using vu32_ptr = volatile uint32_t*;

//Pins

// AN0/A0/RP1 format - Pins led1(A0), Pins led2(RP1, DOUT), Pins pv(AN0)
// m = AIN,DIN,DINPU,DINPD,DINL,DOUT,DOUTL (default is AIN)
// RPN enum encoded as 0xaaaaarrrrrppnnnn (a = ANn, r=RPn, pp=PORT, nnnn=PIN)
//=============================================================================
                    Pins::Pins          (RPN e, IOMODE m)
//=============================================================================
    : m_pt((vu32_ptr)ANSELA + ((e>>PTSHIFT) bitand PTMASK) * ANSELX_SPACING),
      m_pn(1<<(e bitand PNMASK)),
      m_lowison(m bitand ACTL),
      m_rpn((uint8_t)((e>>RPSHIFT) bitand RPMASK)),
      m_ppsin(PPSINOFF),
      m_an((e>>ANSHIFT) bitand ANMASK)
{
    if(m == AIN) analog_in();
    else if(m bitand IN) digital_in();
    else digital_out();
    pullup(m == INPU);
    pulldn(m == INPD);
}

//=============================================================================
    bool        Pins::pinval        () const
//=============================================================================
{
    return Reg::anybit(m_pt + PORT, m_pn);
}

//=============================================================================
    bool        Pins::latval        () const
//=============================================================================
{
    return Reg::anybit(m_pt + LAT, m_pn);
}

//=============================================================================
    void        Pins::latval        (bool tf) const
//=============================================================================
{
    return Reg::setbit(m_pt + LAT, m_pn, tf);
}

//=============================================================================
    uint16_t    Pins::adcval         () const
//=============================================================================
{
    Adc adc;
    adc.mode_12bit(true);           //12bit mode
    adc.trig_sel(adc.AUTO);         //adc starts conversion
    adc.samp_time(31);              //max sampling time- 31Tad
    adc.conv_time(adc.PBCLK12BIT);  //if no arg,default is 4 (for 24MHz)
    adc.ch_sel(m_an);               //ANn (AVss if no ANn for pin)
    adc.on(true);
    adc.samp(true);
    while(not Adc::done());         //blocking
    return Adc::read();             //buf[0]
}

//=============================================================================
    void        Pins::low           () const
//=============================================================================
{
    Reg::clrbit(m_pt + LAT, m_pn);
}

//=============================================================================
    void        Pins::high          () const
//=============================================================================
{
    Reg::setbit(m_pt + LAT, m_pn);
}

//=============================================================================
    void        Pins::invert        () const
//=============================================================================
{
    Reg::flipbit(m_pt + LAT, m_pn);
}

//=============================================================================
    void        Pins::on            () const
//=============================================================================
{
    Reg::setbit(m_pt + LAT, m_pn, not m_lowison);
}

//=============================================================================
    void        Pins::off           () const
//=============================================================================
{
    Reg::setbit(m_pt + LAT, m_pn, m_lowison);
}

//=============================================================================
    bool        Pins::ison          () const
//=============================================================================
{
    return m_lowison ? not pinval() : pinval();
}

//=============================================================================
    void        Pins::icn_flagclr () const
//=============================================================================
{
    Reg::clrbit(m_pt + CNF, m_pn);
}

//=============================================================================
    void        Pins::lowison           (bool tf)
//=============================================================================
{
    m_lowison = tf;
}

//=============================================================================
    void        Pins::digital_in        () const
//=============================================================================
{
    Reg::setbit(m_pt + TRIS, m_pn);
    Reg::clrbit(m_pt, m_pn);
}

//=============================================================================
    void        Pins::analog_in         () const
//=============================================================================
{
    Reg::setbit(m_pt + TRIS, m_pn);
    Reg::setbit(m_pt, m_pn);
}

//=============================================================================
    void        Pins::digital_out       () const
//=============================================================================
{
    Reg::clrbit(m_pt + TRIS, m_pn);
    Reg::clrbit(m_pt, m_pn);
}

//=============================================================================
    void        Pins::odrain            (bool tf) const
//=============================================================================
{
    Reg::setbit(m_pt + ODC, m_pn, tf);
}

//=============================================================================
    void        Pins::pullup            (bool tf) const
//=============================================================================
{
    Reg::setbit(m_pt + CNPU, m_pn, tf);
}

//=============================================================================
    void        Pins::pulldn            (bool tf) const
//=============================================================================
{
    Reg::setbit(m_pt + CNPD, m_pn, tf);
}

//=============================================================================
    void        Pins::icn               (bool tf) const
//=============================================================================
{
    Reg::setbit(m_pt + CNCON, ON, tf);
}

//=============================================================================
    void        Pins::icn_rising        () const
//=============================================================================
{
    Reg::setbit(m_pt + CNCON, CNSTYLE);
    Reg::setbit(m_pt + CNEN0, m_pn);
    Reg::clrbit(m_pt + CNEN1, m_pn);
}

//=============================================================================
    void        Pins::icn_risefall      () const
//=============================================================================
{
    Reg::setbit(m_pt + CNCON, CNSTYLE);
    Reg::setbit(m_pt + CNEN0, m_pn);
    Reg::clrbit(m_pt + CNEN1, m_pn);
}

//=============================================================================
    void        Pins::icn_falling       () const
//=============================================================================
{
    Reg::setbit(m_pt + CNCON, CNSTYLE);
    Reg::setbit(m_pt + CNEN1, m_pn);
    Reg::clrbit(m_pt + CNEN0, m_pn);
}

//=============================================================================
    void        Pins::icn_mismatch      () const
//=============================================================================
{
    Reg::setbit(m_pt + CNEN0, m_pn);
    Reg::clrbit(m_pt + CNCON, CNSTYLE);
}

//=============================================================================
    bool        Pins::icn_flag          () const
//=============================================================================
{
    return Reg::anybit(m_pt + CNF, m_pn);
}

//=============================================================================
    bool        Pins::icn_stat          () const
//=============================================================================
{
    return Reg::anybit(m_pt + CNSTAT, m_pn);
}

//static
//unlock, write byte, lock
//=============================================================================
    void        Pins::pps_do            (uint32_t addr, uint8_t v)
//=============================================================================
{
    Sys::unlock();
    Reg::clrbit(RPCON, IOLOCK);
    Reg::val(addr, v);
    Reg::setbit(RPCON, IOLOCK);
    Sys::lock();
}

//pin -> pps peripheral in, or turn off
//=============================================================================
    void        Pins::pps_in            (PPSIN e)
//=============================================================================
{
    if(m_rpn == 0) return;      //no pps for this pin
    if(e not_eq PPSINOFF) m_ppsin = (uint8_t)e; //save peripheral number
    if(m_ppsin == PPSINOFF) return; //not set previously, nothing to do
    //set peripheral m_ppsin register to 0 if off, or RPn number
    pps_do(RPINR1 + ((m_ppsin / 4) * 16) + (m_ppsin % 4), e == PPSINOFF ? 0 : m_rpn);
    digital_in();
}

//pps peripheral out -> pin
//=============================================================================
    void        Pins::pps_out           (PPSOUT e)
//=============================================================================
{
    if(m_rpn == 0) return; //no pps for this pin
    uint8_t n = m_rpn - 1; //1 based to 0 based to calc reg addresses
    pps_do(RPOR0 + ((n / 4) * 16) + (n % 4), e);
}

//return ANn number for ADC channel select
//=============================================================================
    uint8_t     Pins::an_num            ()
//=============================================================================
{
    return m_an;
}
