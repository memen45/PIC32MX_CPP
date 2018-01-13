#pragma once

#include <cstdint>
#include "Reg.hpp"
#include "Osc.hpp"

/*=============================================================================
 I2c 1/2/3 functions
=============================================================================*/

struct I2c  {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //instantiate I2c with i2c number
    enum I2CX { I2C1 = 0, I2C2, I2C3 };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    constexpr I2c(I2CX);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2C1CON

    enum HOLDTIM : bool { NS100 = 0, NS300 };
    enum ACKBIT : bool { ACK = 0, NACK = 1 };

    void            irq_stop        (bool);             //irq enable on stop
    void            irq_start       (bool);             //irq enable on start
    void            overwrite       (bool);
    void            hold_time       (HOLDTIM);
    void            irq_collision   (bool);
    void            on              (bool);
    void            stop_idle       (bool);
    void            clk_release     (bool);
    void            strict          (bool);
    void            addr_10bit      (bool);
    void            slew_rate       (bool); //0=true
    void            smb_levels      (bool);
    void            irq_gencall     (bool);
    void            clk_stretch     (bool);
    void            ack_bit         (ACKBIT);
    void            ack             (bool);
    void            rx              (bool);
    void            stop            (bool);
    void            repstart        (bool);
    void            start           (bool);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2CXSTAT

    enum DATADDR : bool { ADDR = 0, DATA };

    bool            stat_ack        ();
    bool            stat_txbusy     ();
    bool            stat_acktime    ();
    bool            stat_buscol     ();
    void            stat_buscol_clr ();
    bool            stat_gencall    ();
    bool            stat_10bit      ();
    bool            stat_txcol      ();
    void            stat_txcol_clr  ();
    bool            stat_rxoflow    ();
    void            stat_rxoflow_clr();
    DATADDR         stat_dataddr    ();
    bool            stat_stopbit    ();
    bool            stat_startbit   ();
    bool            stat_rw         ();
    bool            stat_rxfull     ();
    bool            stat_txfull     ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2CXADDR

    void            addr            (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2CXMSK

    void            addr_mask       (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2CXBRG

    enum I2CBAUD : uint32_t {
        KHZ100 = 100000, KHZ400 = 400000, MHZ1 = 1000000
    };

    void            baud            (I2CBAUD);
    void            brg             (uint16_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2CXTRN

    void            tx              (uint8_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //I2CXRCV

    uint8_t         rx              ();

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    Reg r;

    enum :uint32_t {
        I2C1CON = 0xBF801500, I2CX_SPACING = 0x40, //spacing in words
            PCIE = 1<<22,
            SCIE = 1<<21,
            BOEN = 1<<20,
            SDAHT = 1<<19,
            SBCDE = 1<<18,
            ON = 1<<15,
            SIDL = 1<<13,
            SCLREL = 1<<12,
            STRICT = 1<<11,
            A10M = 1<<10,
            DISSLW = 1<<9,
            SMEN = 1<<8,
            GCEN = 1<<7,
            STREN = 1<<6,
            ACKDT = 1<<5,
            ACKEN = 1<<4,
            RCEN = 1<<3,
            PEN = 1<<2,
            RSEN = 1<<1,
            SEN = 1<<0,
        I2CXSTAT = 4, //offset from I2xCON in words
            ACKSTAT = 1<<15,
            TRSTAT = 1<<14,
            ACKTIM = 1<<13,
            BCL = 1<<10,
            GCSTAT = 1<<9,
            ADD10 = 1<<8,
            IWCOL = 1<<7,
            I2COV = 1<<6,
            DA = 1<<5,
            P = 1<<4,
            S = 1<<3,
            RW = 1<<2,
            RBF = 1<<1,
            TBF = 1<<0,
        I2CXADDR = 8,
        I2CXMSK = 12,
        I2CXBRG = 16,
        I2CXTRN = 20,
        I2CXRCV = 24
    };

    volatile uint32_t* m_i2cx_con;
    I2CBAUD m_baud;

};


/*=============================================================================
 inline functions
=============================================================================*/
constexpr I2c::I2c(I2CX e)
    : m_i2cx_con((volatile uint32_t*)I2C1CON+(e*I2CX_SPACING)),
      m_baud(KHZ100)
{}


//I2C1CON
void I2c::irq_stop(bool tf){
    r.setbit(m_i2cx_con, PCIE, tf);
}
void I2c::irq_start(bool tf){
    r.setbit(m_i2cx_con, SCIE, tf);
}
void I2c::overwrite(bool tf){
    r.setbit(m_i2cx_con, BOEN, tf);
}
void I2c::hold_time(HOLDTIM e){
    r.setbit(m_i2cx_con, SDAHT, e);
}
void I2c::irq_collision(bool tf){
    r.setbit(m_i2cx_con, SBCDE, tf);
}
void I2c::on(bool tf){
    //always set brg in case clock changed
    //or baud not changed since init
    baud(m_baud);
    r.setbit(m_i2cx_con, ON, tf);
}
void I2c::stop_idle(bool tf){
    r.setbit(m_i2cx_con, SIDL, tf);
}
void I2c::clk_release(bool tf){
    r.setbit(m_i2cx_con, SCLREL, tf);
}
void I2c::strict(bool tf){
    r.setbit(m_i2cx_con, STRICT, tf);
}
void I2c::addr_10bit(bool tf){
    r.setbit(m_i2cx_con, A10M, tf);
}
void I2c::slew_rate(bool tf){
    r.setbit(m_i2cx_con, DISSLW, !tf);
}
void I2c::smb_levels(bool tf){
    r.setbit(m_i2cx_con, SMEN, tf);
}
void I2c::irq_gencall(bool tf){
    r.setbit(m_i2cx_con, GCEN, tf);
}
void I2c::clk_stretch(bool tf){
    r.setbit(m_i2cx_con, STREN, tf);
}
void I2c::ack_bit(ACKBIT e){
    r.setbit(m_i2cx_con, ACKDT, e);
}
void I2c::ack(bool tf){
    r.setbit(m_i2cx_con, ACKEN, tf);
}
void I2c::rx(bool tf){
    r.setbit(m_i2cx_con, RCEN, tf);
}
void I2c::stop(bool tf){
    r.setbit(m_i2cx_con, PEN, tf);
}
void I2c::repstart(bool tf){
    r.setbit(m_i2cx_con, RSEN, tf);
}
void I2c::start(bool tf){
    r.setbit(m_i2cx_con, SEN, tf);
}

//I2CXSTAT
bool I2c::stat_ack(){
    return r.anybit(m_i2cx_con+I2CXSTAT, ACKSTAT);
}
bool I2c::stat_txbusy(){
    return r.anybit(m_i2cx_con+I2CXSTAT, TRSTAT);
}
bool I2c::stat_acktime(){
    return r.anybit(m_i2cx_con+I2CXSTAT, ACKTIM);
}
bool I2c::stat_buscol(){
    return r.anybit(m_i2cx_con+I2CXSTAT, BCL);
}
void I2c::stat_buscol_clr(){
    r.clrbit(m_i2cx_con+I2CXSTAT, BCL);
}
bool I2c::stat_gencall(){
    return r.anybit(m_i2cx_con+I2CXSTAT, GCSTAT);
}
bool I2c::stat_10bit(){
    return r.anybit(m_i2cx_con+I2CXSTAT, ADD10);
}
bool I2c::stat_txcol(){
    return r.anybit(m_i2cx_con+I2CXSTAT, IWCOL);
}
void I2c::stat_txcol_clr(){
    r.clrbit(m_i2cx_con+I2CXSTAT, IWCOL);
}
bool I2c::stat_rxoflow(){
    return r.anybit(m_i2cx_con+I2CXSTAT, I2COV);
}
void I2c::stat_rxoflow_clr(){
    r.clrbit(m_i2cx_con+I2CXSTAT, I2COV);
}
auto I2c::stat_dataddr() -> DATADDR {
    return (DATADDR)r.anybit(m_i2cx_con+I2CXSTAT, DA);
}
bool I2c::stat_stopbit(){
    return r.anybit(m_i2cx_con+I2CXSTAT, P);
}
bool I2c::stat_startbit(){
    return r.anybit(m_i2cx_con+I2CXSTAT, S);
}
bool I2c::stat_rw(){
    return r.anybit(m_i2cx_con+I2CXSTAT, RW);
}
bool I2c::stat_rxfull(){
    return r.anybit(m_i2cx_con+I2CXSTAT, RBF);
}
bool I2c::stat_txfull(){
    return r.anybit(m_i2cx_con+I2CXSTAT, TBF);
}

//I2CXADDR
void I2c::addr(uint16_t v){
    r.val(m_i2cx_con+I2CXADDR, v);
}

//I2CXMSK
void I2c::addr_mask(uint16_t v){
    r.val(m_i2cx_con+I2CXMSK, v);
}

//I2CXBRG
void I2c::baud(I2CBAUD e){
    uint32_t sck = e * 2;
    uint32_t clk = Osc::sysclk()<<6;
    clk = clk/sck - clk/9615384; //1/9615384=104ns=Tpgd
    brg((clk>>6)-2);
    m_baud = e;
}
void I2c::brg(uint16_t v){
    if(v < 2) v = 2; //0,1 not allowed
    r.val(m_i2cx_con+I2CXBRG, v);
}

//I2CXTRN
void I2c::tx(uint8_t v){
    r.val(m_i2cx_con+I2CXTRN, v);
}

//I2CXRCV
uint8_t I2c::rx(){
    return r.val8(m_i2cx_con+I2CXRCV);
}
