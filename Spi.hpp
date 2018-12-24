#pragma once

#include <cstdint>
#include "Pins.hpp"

//SPI 1/2/3

struct Spi  {

    //internal SPI register order used here
    enum SPIX { SPI3, SPI2, SPI4, SPI1 };

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
            
            //true: disable sdo pin
            auto
dis_sdo     (bool) -> void;

            enum
MODE        : uint8_t {
            MODE8 = 0, MODE16, MODE32,
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
CLKPOL      : bool { CLKH, CLKL };   //CLKH --> Idle Low, Active High | CLKL --> Idle High, Active Low

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

            private:

            volatile uint32_t*  m_spix_con;
            volatile uint32_t&  m_spixbuf;      //use reference
            uint32_t            m_spix_freq;    //set to actual spi freq

};
