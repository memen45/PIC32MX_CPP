#pragma once

#include <cstdint>
#include "Pins.hpp"
#include "Reg.hpp"

//SPI 1/2/3

struct Spi : private Reg {

            enum
SPIX        { SPI1, SPI2, SPI3 };

            //instantiate spi number
            //SPI2 uses PPS, so user will also have to setup pins
            //(not done here as there are a number of pins, and not all always needed)
Spi         (SPIX);

            //==== SPIxCON ====

            //framed support
            auto
frame       (bool) -> void;

            enum
FRMDIR      : bool { MASTER, SLAVE };

            //frame sync pulse dir
            auto
frame_dir   (FRMDIR) -> void;

            enum
FRMHL       : bool { LOW, HIGH };

            //frame sync polarity
            auto
frame_pol   (FRMHL) -> void;

            //slave select enable
            auto
slave_sel   (bool) -> void;

            enum
FRMPW       : bool { CLKW, CHARW };

            //1=1char, 0=1clk
            auto
frame_pwidth (FRMPW) -> void;

            enum
FRMCNT      : uint8_t { CNT1, CNT2, CNT4, CNT8, CNT16, CNT32 };

            //frame sync counter
            auto
frame_count (FRMCNT) -> void;

            enum
CLKSEL      : bool { PBCLK, REFO1 };

            //set clock source
            auto
clk_sel     (CLKSEL) -> void;

            //get clock source
            auto
clk_sel     () -> CLKSEL;

            enum
FRMEDGE     : bool { B4BCLK, ATBCLK };

            //frame sync edge sel
            auto
frame_edge  (FRMEDGE) -> void;

            //enhanced buffer mode
            auto
enhanced    (bool) -> void;

            //spi on/off
            auto
on          (bool) -> void;

            enum
MODE        : uint8_t {
            MODE8 = 0, MODE16, MODE32,
            AMODE1616 = 0, AMODE1632, AMODE3232, AMODE2432
            };

            //set spi mode
            auto
mode        (MODE) -> void;

            enum
PHASE       : bool { SMPMID, SMPEND };

            //sample phase bit
            auto
phase       (PHASE) -> void;

            enum
CLKEDGE     : bool { LEAD, TRAIL };

            //clk edge sel
            auto
clk_edge    (CLKEDGE) -> void;

            //slave select enable
            auto
ss          (bool) -> void;

            enum
CLKPOL      : bool { CLKH, CLKL };

            //clock polarity
            auto
clk_pol     (CLKPOL) -> void;

            //master mode
            auto
master      (bool) -> void;

            enum
TXIRQ       : uint8_t { TDONE, TEMPTY, THALF, TNOTFULL };

            //tx irq mode
            auto
tx_irq      (TXIRQ) -> void;

            enum
RXIRQ       : uint8_t { REMPTY, RANY, RHALF, RFULL };

            //rx irq mode
            auto
rx_irq      (RXIRQ) -> void;


            //==== SPIxSTAT ====

            //enhanced rx buf count
            auto
stat_rxcount () -> uint8_t;

            //enhanced tx buf count
            auto
stat_txcount () -> uint8_t;

            //framing error?
            auto
stat_ferr   () -> bool;

            //framing error clear
            auto
stat_ferrclr () -> void;

            //spi busy?
            auto
stat_busy   () -> bool;

            //tx underrun error?
            auto
stat_txurun () -> bool;

            //shift reg empty?
            auto
stat_sremty () -> bool;

            //rx overflow?
            auto
stat_oerr   () -> bool;

            //rx overflow clear
            auto
stat_oerrclr () -> void;

            //rx empty?
            auto
stat_rxemty () -> bool;

            //tx empty?
            auto
stat_txemty () -> bool;

            //tx full?
            auto
stat_txfull () -> bool;

            //rx full?
            auto
stat_rxfull () -> bool;


            //==== SPIxBUF ====

            //write buf 8/16/32bit
            auto
write       (uint32_t) -> void;

            //read buf 8/16/32bit
            auto
read        () -> uint32_t;


            //==== SPIxBRG ====

            //set baud
            auto
baud        (uint16_t) -> void;

            //set frequency
            auto
freq        (uint32_t) -> void;

            //get frequency
            auto
freq        () -> uint32_t;


            //==== SPIxCON2 ====

            //rx sign extend
            auto
sign_ext    (bool) -> void;

            //frame error irq
            auto
irq_frmerr  (bool) -> void;

            //overflow error irq
            auto
irq_oflow   (bool) -> void;

            //underrun err irq
            auto
irq_urun    (bool) -> void;

            //ignore overflow
            auto
ign_oflow   (bool) -> void;

            //ignore underrun
            auto
ign_urun    (bool) -> void;

            //audio mode
            auto
audio       (bool) -> void;

            //audio mono
            auto
mono        (bool) -> void;

            enum
AUDMOD      : uint8_t { I2S, LEFT, RIGHT, PCMDSP };

            //audio mode
            auto
audio_mode  (AUDMOD) -> void;


            private:

            volatile uint32_t*  m_spix_con;
            volatile uint32_t&  m_spixbuf;      //use reference
            uint32_t            m_spix_freq;    //set to actual spi freq

};
