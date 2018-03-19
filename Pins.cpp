#include "Pins.hpp"
#include "Adc.hpp"
#include "Reg.hpp"
#include "Sys.hpp"

enum { //offsets from base address, in words
    TRIS = 0, PORT = 4, LAT = 8, ODC = 12
};

enum {
	TRISX_SPACING = 16, //spacing in words
	TRISA = 0xBF886000,
	AD1PCFG = 0xBF809060,
    
	CNCON = 0xBF8861C0,
	CNEN = 0xBF8861D0,
	CNPUE = 0xBF8861E0,
	
    //CNCONx
        ON = 15
};

//IOMODE
// bit  | 4  | 3  |  2   |  1   |  0  |
//      | PD | PU | ACTL | DOUT | DIN |
//-----------------------------------------
//AIN   |    |    |      |      |     | 0
//IN    |    |    |      |      |  1  | 1
//INPU  |    |  1 |  1   |      |  1  | 13
//INPD  |  1 |    |      |      |  1  | 17  //not supported
//INL   |    |    |  1   |      |  1  | 5
//OUT   |    |    |      |   1  |     | 2
//OUTL  |    |    |  1   |   1  |     | 6

enum : uint8_t { ACTL = 1<<2  }; //IOMODE ACTL bit (active low bit)

//Pins

// PIN11/AN0/A0/RP1 format - Pins led1(A0), Pins led2(RP1, DOUT), Pins pv(AN0)
// m = AIN,DIN,DINPU,DINPD,DINL,DOUT,DOUTL (default is AIN)
// RPN enum encoded as 0xaaaacccccpppnnnn (a = ANn, c = CNn, pp = PORT, nnnn = PIN
//=============================================================================
                    Pins::Pins          (RPN e, IOMODE m)
//=============================================================================
    : m_pt((vu32ptr)TRISA + ((e>>PTSHIFT) bitand PTMASK) * TRISX_SPACING),
      m_pn(1<<(e bitand PNMASK)),
      m_lowison(m bitand ACTL),
      m_an((e>>ANSHIFT) bitand ANMASK),
	  m_cn((e>>CNSHIFT) bitand CNMASK)
{
    if(m == AIN) analog_in(); 
    else if(m bitand IN) digital_in();
    else digital_out();
	
    if (m_cn != NO_CN)
		pullup(m == INPU);
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
    Adc::trig_sel(Adc::AUTO);        //adc starts conversion
    Adc::samp_time(31);              //max sampling time- 31Tad
    Adc::conv_time(3);               //if no arg,default is 7 (for 80MHz)
    Adc::ch_sel((Adc::CH0S)m_an);   //ANn
    Adc::on(true);
    Adc::samp(true);
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
    if (m_pt == ((vu32ptr)TRISA + TRISX_SPACING))
        Reg::setbit(AD1PCFG, 1 << m_an);
}

//=============================================================================
    void        Pins::analog_in         () const
//=============================================================================
{
    Reg::setbit(m_pt + TRIS, m_pn);
    if (m_pt == ((vu32ptr)TRISA + TRISX_SPACING))
        Reg::clrbit(AD1PCFG, 1 << m_an);
}

//=============================================================================
    void        Pins::digital_out       () const
//=============================================================================
{
    Reg::clrbit(m_pt + TRIS, m_pn);
    if (m_pt == ((vu32ptr)TRISA + TRISX_SPACING))
            Reg::setbit(AD1PCFG, 1 << m_an);
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
    Reg::setbit(CNPUE, m_pn, tf);
}

//=============================================================================
    void        Pins::icn               (bool tf) const
//=============================================================================
{
    Reg::setbit(CNCON, 1<<ON, tf);
}

//return ANn number for ADC channel select
//=============================================================================
    uint8_t     Pins::an_num            ()
//=============================================================================
{
    return m_an;
}

//return CNn number for CN select
//=============================================================================
    uint8_t     Pins::cn_num            ()
//=============================================================================
{
    return m_cn;
}

