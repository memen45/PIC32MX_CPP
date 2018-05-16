#pragma once

#include <cstdint>

//SPI 1/2/3

struct Spi  {

        enum
    SPIX { SPI1, SPI2, SPI3 };

    Spi(SPIX);      //instantiate Comp123 with comparator number

    //spixcon

        auto
    frame (bool) -> void;               //framed support

        enum
    FRMDIR : bool { MASTER, SLAVE };

        auto
    frame_dir (FRMDIR) -> void;         //frame sync pulse dir

        enum
    FRMHL : bool { LOW, HIGH };

        auto
    frame_pol (FRMHL) -> void;          //frame sync polarity

        auto
    slave_sel (bool) -> void;           //slave select enable

        enum
    FRMPW : bool { CLKW, CHARW };

        auto
    frame_pwidth (FRMPW) -> void;       //1=1char, 0=1clk

        enum
    FRMCNT : uint8_t { CNT1, CNT2, CNT4, CNT8, CNT16, CNT32 };

        auto
    frame_count (FRMCNT) -> void;       //frame sync counter

        enum
    CLKSEL : bool { PBCLK, REFO1 };

        auto
    clk_sel (CLKSEL) -> void;           //set clock source

        auto
    clk_sel () -> CLKSEL;               //get clock source

        enum
    FRMEDGE : bool { B4BCLK, ATBCLK };

        auto
    frame_edge (FRMEDGE) -> void;       //frame sync edge sel

        auto
    enhanced (bool) -> void;            //enhanced buffer mode

        auto
    on (bool) -> void;                  //spi on/off

        enum
    MODE : uint8_t {
        MODE8 = 0, MODE16, MODE32,
        AMODE1616 = 0, AMODE1632, AMODE3232, AMODE2432
        };

        auto
    mode (MODE) -> void;                //set spi mode

        enum
    PHASE: bool { SMPMID, SMPEND };

        auto
    phase (PHASE) -> void;              //sample phase bit

        enum
    CLKEDGE : bool { LEAD, TRAIL };

        auto
    clk_edge (CLKEDGE) -> void;         //clk edge sel

        auto
    ss (bool) -> void;                  //slave select enable

        enum
    CLKPOL : bool { CLKH, CLKL };

        auto
    clk_pol (CLKPOL) -> void;           //clock polarity

        auto
    master (bool) -> void;              //master mode

        enum
    TXIRQ : uint8_t { TDONE, TEMPTY, THALF, TNOTFULL };

        auto
    tx_irq (TXIRQ) -> void;             //tx irq mode

        enum
    RXIRQ : uint8_t { REMPTY, RANY, RHALF, RFULL };

        auto
    rx_irq (RXIRQ) -> void;             //rx irq mode

    //spixstat

        auto
    stat_rxcount () -> uint8_t;         //enhanced rx buf count

        auto
    stat_txcount () -> uint8_t;         //enhanced tx buf count

        auto
    stat_ferr () -> bool;               //framing error

        auto
    stat_ferrclr () -> void;            //framing error clear

        auto
    stat_busy () -> bool;               //spi busy

        auto
    stat_txurun () -> bool;             //tx underrun error

        auto
    stat_sremty () -> bool;             //shift reg empty

        auto
    stat_oerr () -> bool;               //rx overflow

        auto
    stat_oerrclr () -> void;            //rx overflow clear

        auto
    stat_rxemty () -> bool;             //rx empty

        auto
    stat_txemty () -> bool;             //tx empty

        auto
    stat_txfull () -> bool;             //tx full

        auto
    stat_rxfull () -> bool;             //rx full

    //spixbuf

        auto
    write (uint32_t) -> void;           //set buf 8/16/32bit

        auto
    read () -> uint32_t;                //get buf 8/16/32bit

    //spixbrg

        auto
    baud (uint16_t) -> void;            //set baud

        auto
    freq (uint32_t) -> void;            //set frequency

        auto
    freq () -> uint32_t;                //get frequency

    //spixcon2

        auto
    sign_ext (bool) -> void;            //rx sign extend

        auto
    irq_frmerr (bool) -> void;          //frame error irq

        auto
    irq_oflow (bool) -> void;           //overflow error irq

        auto
    irq_urun (bool) -> void;            //underrun err irq

        auto
    ign_oflow (bool) -> void;           //ignore overflow

        auto
    ign_urun (bool) -> void;            //ignore underrun

        auto
    audio (bool) -> void;               //audio mode

        auto
    mono (bool) -> void;                //audio mono

        enum
    AUDMOD : uint8_t { I2S, LEFT, RIGHT, PCMDSP };

        auto
    audio_mode (AUDMOD) -> void;        //audio mode


    private:

    volatile uint32_t*  m_spix_con;
    volatile uint32_t&  m_spixbuf;      //use reference
    uint32_t            m_spix_freq;    //set to actual spi freq

};
