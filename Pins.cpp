#include "Pins.hpp"
#include "Adc.hpp"
#include "Reg.hpp"
#include "Sys.hpp"

enum { //offsets from base address, in words
TRIS = 4,
PORT = 8,
LAT = 12,
ODC = 16,
CNPU = 20,
CNPD = 24,
CNCON = 28,
CNEN0 = 32,
CNSTAT = 36,
CNEN1 = 40,
CNF = 44
};

enum {
ANSELX_SPACING = 64, //spacing in words
ANSELA = 0xBF802BB0,
//CNCONx
    ON = 15,
    CNSTYLE = 11,
RPCON = 0xBF802A00,
    IOLOCK = 11,
RPINR1 = 0xBF802A20,
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

//Pins

// PIN11/AN0/A0/RP1 format - Pins led1(A0), Pins led2(RP1, DOUT), Pins pv(AN0)
// m = AIN,DIN,DINPU,DINPD,DINL,DOUT,DOUTL (default is AIN)
// RPN enum encoded as 0xaaaaarrrrrppnnnn (a = ANn, r=RPn, pp=PORT, nnnn=PIN)
//=============================================================================
            Pins::
Pins        (RPN e, IOMODE m)
            :
            m_pt((vu32ptr)ANSELA+((e>>PTSHIFT) bitand PTMASK)*ANSELX_SPACING),
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
            auto Pins::
pinval      () -> bool
            {
            return anybit(m_pt + PORT, m_pn);
            }

//=============================================================================
            auto Pins::
latval      () -> bool
            {
            return anybit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
latval      (bool tf) -> void
            {
            return setbit(m_pt + LAT, m_pn, tf);
            }

//=============================================================================
            auto Pins::
adcval      () -> uint16_t
            {
            Adc adc;
            adc.mode_12bit(true);           //12bit mode
            adc.format(adc.INT16);          //integer
            adc.trig_sel(adc.AUTO);         //adc starts conversion
            adc.samp_time(31);              //max sampling time- 31Tad
            adc.conv_time();                //if no arg,default is 4 (for 24MHz)
            adc.ch_sel((Adc::CH0SA)m_an);   //ANn (AVss if no ANn for pin)
            adc.on(true);
            adc.samp(true);
            while(not Adc::done());         //blocking
            adc.on(false);
            return (uint16_t)Adc::read();   //default read is from BUF0
            }

//=============================================================================
            auto Pins::
low         () -> void
            {
            clrbit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
high        () -> void
            {
            setbit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
invert      () -> void
            {
            flipbit(m_pt + LAT, m_pn);
            }

//=============================================================================
            auto Pins::
on          () -> void
            {
            setbit(m_pt + LAT, m_pn, not m_lowison);
            }

//=============================================================================
            auto Pins::
off         () -> void
            {
            setbit(m_pt + LAT, m_pn, m_lowison);
            }

//=============================================================================
            auto Pins::
ison        () -> bool
            {
            return m_lowison xor pinval();
            }

//=============================================================================
            auto Pins::
icn_flagclr () -> void
            {
            clrbit(m_pt + CNF, m_pn);
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
            setbit(m_pt + TRIS, m_pn);
            clrbit(m_pt, m_pn); //ANSELx
            }

//=============================================================================
            auto Pins::
analog_in   () -> void
            {
            setbit(m_pt + TRIS, m_pn);
            setbit(m_pt, m_pn); //ANSELx
            }

//=============================================================================
            auto Pins::
digital_out () -> void
            {
            clrbit(m_pt + TRIS, m_pn);
            clrbit(m_pt, m_pn); //ANSELx
            }

//=============================================================================
            auto Pins::
odrain      (bool tf) -> void
            {
            setbit(m_pt + ODC, m_pn, tf);
            }

//=============================================================================
            auto Pins::
pullup      (bool tf) -> void
            {
            setbit(m_pt + CNPU, m_pn, tf);
            }

//=============================================================================
            auto Pins::
pulldn      (bool tf) -> void
            {
            setbit(m_pt + CNPD, m_pn, tf);
            }

//=============================================================================
            auto Pins::
icn         (bool tf) -> void
            {
            setbit(m_pt + CNCON, 1<<ON, tf);
            }

//=============================================================================
            auto Pins::
icn_rising  () -> void
            {
            setbit(m_pt + CNCON, 1<<CNSTYLE);
            setbit(m_pt + CNEN0, m_pn);
            clrbit(m_pt + CNEN1, m_pn);
            }

//=============================================================================
            auto Pins::
icn_risefall () -> void
            {
            setbit(m_pt + CNCON, 1<<CNSTYLE);
            setbit(m_pt + CNEN0, m_pn);
            setbit(m_pt + CNEN1, m_pn);
            }

//=============================================================================
            auto Pins::
icn_falling () -> void
            {
            setbit(m_pt + CNCON, 1<<CNSTYLE);
            setbit(m_pt + CNEN1, m_pn);
            clrbit(m_pt + CNEN0, m_pn);
            }

//=============================================================================
            auto Pins::
icn_mismatch () -> void
            {
            setbit(m_pt + CNEN0, m_pn);
            clrbit(m_pt + CNCON, 1<<CNSTYLE);
            }

//=============================================================================
            auto Pins::
icn_flag    () -> bool
            {
            return anybit(m_pt + CNF, m_pn);
            }

//=============================================================================
            auto Pins::
icn_stat    () -> bool
            {
            return anybit(m_pt + CNSTAT, m_pn);
            }

//static
//unlock, write byte, lock
//=============================================================================
            auto Pins::
pps_do      (uint32_t addr, uint8_t v) -> void
            {
            Sys::unlock();
            clrbit(RPCON, 1<<IOLOCK);
            val(addr, v);
            setbit(RPCON, 1<<IOLOCK);
            Sys::lock();
            }

//pin -> pps peripheral in, or turn off
//=============================================================================
            auto Pins::
pps_in      (PPSIN e) -> void
            {
            if(m_rpn == 0) return;      //no pps for this pin
            if(e not_eq PPSINOFF) m_ppsin = (uint8_t)e; //save peripheral number
            if(m_ppsin == PPSINOFF) return; //not set previously, nothing to do
            //set peripheral m_ppsin register to 0 if off, or RPn number
            pps_do(RPINR1 + m_ppsin, e == PPSINOFF ? 0 : m_rpn);
            digital_in();
            }

//pps peripheral out -> pin
//=============================================================================
            auto Pins::
pps_out     (PPSOUT e) -> void
            {
            if(m_rpn == 0) return; //no pps for this pin
            uint8_t n = m_rpn - 1; //1 based to 0 based to calc reg addresses
            pps_do(RPOR0 + ((n / 4) * 16) + (n % 4), e);
            }

//return ANn number for ADC channel select
//=============================================================================
            auto Pins::
an_num      () -> uint8_t
            {
            return m_an;
            }
