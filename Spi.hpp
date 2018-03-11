#pragma once

#include <cstdint>

//SPI 1/2/3

struct Spi  {

    //instantiate Comp123 with comparator number
    enum SPIX { SPI1, SPI2, SPI3 };
    Spi(SPIX);

    //spixcon
    void            frame           (bool);             //framed support

    enum FRMDIR : bool { MASTER, SLAVE };
    void            frame_dir       (FRMDIR);           //frame sync pulse dir

    enum FRMHL : bool { LOW, HIGH };
    void            frame_pol       (FRMHL);            //frame sync polarity
    void            slave_sel       (bool);             //slave select enable

    enum FRMPW : bool { CLKW, CHARW };
    void            frame_pwidth    (FRMPW);            //1=1char, 0=1clk

    enum FRMCNT : uint8_t { CNT1, CNT2, CNT4, CNT8, CNT16, CNT32 };
    void            frame_count     (FRMCNT);           //frame sync counter

    enum CLKSEL : bool { PBCLK, REFO1 };
    void            clk_sel         (CLKSEL);           //set clock source
    CLKSEL          clk_sel         ();                 //get clock source

    enum FRMEDGE : bool { B4BCLK, ATBCLK };
    void            frame_edge      (FRMEDGE);          //frame sync edge sel

    void            enhanced        (bool);             //enhanced buffer mode
    void            on              (bool);             //spi on/off

    enum MODE : uint8_t {
        MODE8 = 0, MODE16, MODE32,
        AMODE1616 = 0, AMODE1632, AMODE3232, AMODE2432
    };
    void            mode            (MODE);             //set spi mode

    enum PHASE: bool { SMPMID, SMPEND };
    void            phase           (PHASE);            //sample phase bit

    enum CLKEDGE : bool { LEAD, TRAIL };
    void            clk_edge        (CLKEDGE);          //clk edge sel

    void            ss              (bool);             //slave select enable

    enum CLKPOL : bool { CLKH, CLKL };
    void            clk_pol         (CLKPOL);           //clock polarity

    void            master          (bool);             //master mode

    enum TXIRQ : uint8_t { TDONE, TEMPTY, THALF, TNOTFULL };
    void            tx_irq          (TXIRQ);            //tx irq mode

    enum RXIRQ : uint8_t { REMPTY, RANY, RHALF, RFULL };
    void            rx_irq          (RXIRQ);            //rx irq mode

    //spixstat
    uint8_t         stat_rxcount    ();                 //enhanced rx buf count
    uint8_t         stat_txcount    ();                 //enhanced tx buf count
    bool            stat_ferr       ();                 //framing error
    void            stat_ferrclr    ();                 //framing error clear
    bool            stat_busy       ();                 //spi busy
    bool            stat_txurun     ();                 //tx underrun error
    bool            stat_sremty     ();                 //shift reg empty
    bool            stat_oerr       ();                 //rx overflow
    void            stat_oerrclr    ();                 //rx overflow clear
    bool            stat_rxemty     ();                 //rx empty
    bool            stat_txemty     ();                 //tx empty
    bool            stat_txfull     ();                 //tx full
    bool            stat_rxfull     ();                 //rx full

    //spixbuf
    void            write           (uint32_t);         //set buf 8/16/32bit
    uint32_t        read            ();                 //get buf 8/16/32bit

    //spixbrg
    void            baud            (uint16_t);         //set baud
    void            freq            (uint32_t);         //set frequency
    uint32_t        freq            ();                 //get frequency

    //spixcon2
    void            sign_ext        (bool);             //rx sign extend
    void            irq_frmerr      (bool);             //frame error irq
    void            irq_oflow       (bool);             //overflow error irq
    void            irq_urun        (bool);             //underrun err irq
    void            ign_oflow       (bool);             //ignore overflow
    void            ign_urun        (bool);             //ignore underrun
    void            audio           (bool);             //audio mode
    void            mono            (bool);             //audio mono

    enum AUDMOD : uint8_t { I2S, LEFT, RIGHT, PCMDSP };
    void            audio_mode      (AUDMOD);           //audio mode

    private:

    volatile uint32_t* m_spix_con;
    volatile uint32_t& m_spixbuf;                   //use reference

    uint32_t m_spix_freq;                           //set to actual spi freq

};
