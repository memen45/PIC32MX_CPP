#pragma once

#include <cstdint>
#include "Reg.hpp"

//SPI 1/2/3

struct Spi  {

    //instantiate Comp123 with comparator number
    enum SPIX { SPI1 = 0, SPI2, SPI3 };

    /*constexpr*/ Spi(SPIX);

    //spixcon
    enum FRMDIR : bool { MASTER = 0, SLAVE };
    enum FRMHL : bool { LOW = 0, HIGH };
    enum FRMPW : bool { CLKW = 0, CHARW };
    enum FRMCNT : uint8_t { CNT1 = 0, CNT2, CNT4, CNT8, CNT16, CNT32 };
    enum CLKSEL : bool { PBCLK = 0, REFO1 };
    enum FRMEDGE : bool { B4BCLK = 0, ATBCLK };
    enum PHASE: bool { SMPMID = 0,SMPEND };
    enum CLKEDGE : bool { LEAD = 0, TRAIL };
    enum CLKPOL : bool { CLKH = 0, CLKL };
    enum MODE : uint8_t {
        MODE8 = 0, MODE16, MODE32,
        AMODE1616 = 0, AMODE1632, AMODE3232, AMODE2432
    };
    enum TXIRQ : uint8_t { TDONE = 0, TEMPTY, THALF, TNOTFULL };
    enum RXIRQ : uint8_t { REMPTY = 0, RANY, RHALF, RFULL };

    void            frame           (bool);             //framed support
    void            frame_dir       (FRMDIR);           //frame sync pulse dir
    void            frame_pol       (FRMHL);            //frame sync polarity
    void            slave_sel       (bool);             //slave select enable
    void            frame_pwidth    (FRMPW);            //1=1char, 0=1clk
    void            frame_count     (FRMCNT);           //frame sync counter
    void            clk_sel         (CLKSEL);           //set clock source
    CLKSEL          clk_sel         ();                 //get clock source
    void            frame_edge      (FRMEDGE);          //frame sync edge sel
    void            enhanced        (bool);             //enhanced buffer mode
    void            on              (bool);             //spi on/off
    void            mode            (MODE);             //set spi mode
    void            phase           (PHASE);            //sample phase bit
    void            clk_edge        (CLKEDGE);          //clk edge sel
    void            ss              (bool);             //slave select enable
    void            clk_pol         (CLKPOL);           //clock polarity
    void            master          (bool);             //master mode
    void            tx_irq          (TXIRQ);            //tx irq mode
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
    enum AUDMOD : uint8_t { I2S = 0, LEFT, RIGHT, PCMDSP };

    void            sign_ext        (bool);             //rx sign extend
    void            irq_frmerr      (bool);             //frame error irq
    void            irq_oflow       (bool);             //overflow error irq
    void            irq_urun        (bool);             //underrun err irq
    void            ign_oflow       (bool);             //ignore overflow
    void            ign_urun        (bool);             //ignore underrun
    void            audio           (bool);             //audio mode
    void            mono            (bool);             //audio mono
    void            audio_mode      (AUDMOD);           //audio mode

    private:

    Reg r;

    enum :uint32_t {
        SPI1CON = 0xBF808100, SPIX_SPACING = 0x40, //spacing in words
            FRMEN = 1u<<31,
            FRMSYNC = 1<<30,
            FRMPOL = 1<<29,
            MSSEN = 1<<28,
            FRMSYPW = 1<<27,
            FRMCNT_SHIFT = 24, FRMCNT_CLR = 7,
            MCLKSEL = 1<<23,
            SPIFE = 1<<17,
            ENHBUF = 1<<16,
            ON = 1<<15,
            MODE_SHIFT = 10, MODE_CLR = 3,
            SMP = 1<<9,
            CKE = 1<<8,
            SSEN = 1<<7,
            CKP = 1<<6,
            MSTEN = 1<<5,
            STXISEL_SHIFT = 2, STXISEL_CLR = 3,
            SRXISEL_SHIFT = 0, SRXISEL_CLR = 3,
        SPIXSTAT = 4, //offset from SPIXCON in words
            FRMERR = 1<<12,
            SPIBUSY = 1<<11,
            SPITUR = 1<<8,
            SRMT = 1<<7,
            SPIROV = 1<<6,
            SPIRBE = 1<<5,
            SPITBE = 1<<3,
            SPITBF = 1<<1,
            SPIRBF = 1<<0,
        SPIXBUF = 8, //offset from SPIXCON in words
        SPIXBRG = 12, //offset from SPIXCON in words
        SPIXCON2 = 16, //offset from SPIXCON in words
            SPISGNEXT = 1<<15,
            FRMERREN = 1<<12,
            SPIROVEN = 1<<11,
            SPITUREN = 1<<10,
            IGNROV = 1<<9,
            IGNTUR = 1<<8,
            AUDEN = 1<<7,
            AUDOMONO = 1<<3
    };

    volatile uint32_t* m_spix_con;
    volatile uint32_t& m_spixbuf;                   //use reference

    uint32_t m_spix_freq;                           //set to actual spi freq

    using vbyte_ptr = volatile uint8_t*;            //access stat as bytes

};
