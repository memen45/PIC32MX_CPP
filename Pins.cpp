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
            Pins::
Pins        (RPN e, IOMODE m)
            : 
            m_pt((vu32ptr)TRISA + ((e>>PTSHIFT) bitand PTMASK) * TRISX_SPACING),
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
            auto Pins::
pinval      () -> bool
            {
            return Reg::anybit(m_pt + PORT, m_pn);
            }

//=============================================================================
            auto Pins::
latval      () -> bool
            {
            return Reg::anybit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
latval      (bool tf) -> void
            {
            return Reg::setbit(m_pt + LAT, m_pn, tf);
            }

//=============================================================================
            auto Pins::
adcval      () -> uint16_t
            {
            Adc adc;
            adc.format(adc.INT16);          //integer
            adc.trig_sel(adc.AUTO);         //adc starts conversion
            adc.samp_time(31);              //max sampling time- 31Tad
            adc.conv_time();                //if no arg,default is 4 (for 24MHz)
            adc.ch_sel((Adc::CH0S)m_an);   //ANn (AVss if no ANn for pin)
            adc.on(true);
            adc.samp(true);
            while(not Adc::done());         //blocking
            return (uint16_t)Adc::read();   //buf[0]
            }

//=============================================================================
            auto Pins::
low         () -> void
            {
            Reg::clrbit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
high        () -> void
            {
            Reg::setbit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
invert      () -> void
            {
            Reg::flipbit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
on          () -> void
            {
            Reg::setbit(m_pt + LAT, m_pn, not m_lowison);
            }

//=============================================================================
            auto Pins::
off         () -> void
            {
            Reg::setbit(m_pt + LAT, m_pn, m_lowison);
            }

//=============================================================================
            auto Pins::
ison        () -> bool
            {
            return m_lowison xor pinval();
            }

//=============================================================================
            auto Pins::
lowison     (bool tf) -> void
            {
            m_lowison = tf;
            }

//=============================================================================
            auto Pins::
digital_in  () -> void
            {
            Reg::setbit(m_pt + TRIS, m_pn);
            if (m_pt == ((vu32ptr)TRISA + TRISX_SPACING))
                Reg::setbit(AD1PCFG, 1 << m_an);
            }

//=============================================================================
            auto Pins::
analog_in   () -> void
            {
            Reg::setbit(m_pt + TRIS, m_pn);
            if (m_pt == ((vu32ptr)TRISA + TRISX_SPACING))
                Reg::clrbit(AD1PCFG, 1 << m_an);
            }

//=============================================================================
            auto Pins::
digital_out () -> void
            {
            Reg::clrbit(m_pt + TRIS, m_pn);
            if (m_pt == ((vu32ptr)TRISA + TRISX_SPACING))
                Reg::setbit(AD1PCFG, 1 << m_an);
            }

//=============================================================================
            auto Pins::
odrain      (bool tf) -> void
            {
            Reg::setbit(m_pt + ODC, m_pn, tf);
            }

            auto Pins::
pullup      (bool tf) -> void
            {
            Reg::setbit(CNPUE, m_pn, tf);
            }

//=============================================================================
            auto Pins::
icn         (bool tf) -> void
            {
            Reg::setbit(CNCON, 1<<ON, tf);
            }

//return ANn number for ADC channel select
//=============================================================================
            auto Pins::
an_num      () -> uint8_t
            {
            return m_an;
            }
            
//return CNn number for CN select
//=============================================================================
            auto Pins::
cn_num      () -> uint8_t
            {
            return m_cn;
            }
            
