#pragma once

//I/O pins

#include <cstdint>

struct Pins {

            //include specified file depending on which pin count device is in use
            //(fyi compiler does not define __PIC32_PIN_COUNT for pic32mm,
            // so just using the compiler provided part defines to get pin count)

            //see specified file for pin information
            //64pin
            #if   (defined __32MM0256GPM064__) || \
                  (defined __32MM0128GPM064__) || \
                  (defined __32MM0064GPM064__)
                #include "pins64.txt"
            //48pin
            #elif (defined __32MM0256GPM048__) || \
                  (defined __32MM0128GPM048__) || \
                  (defined __32MM0064GPM048__)
                //#include "pins48.txt"
                #error "pins48.txt not defined yet, see Pins.h"
            //36pin
            #elif (defined __32MM0256GPM036__) || \
                  (defined __32MM0128GPM036__) || \
                  (defined __32MM0064GPM036__)
                //#include "pins36.txt"
                #error "pins36.txt not defined yet, see Pins.h"
            //28pin (specify SSOP or QFN in pins28.txt)
            #elif (defined __32MM0256GPM028__) || \
                  (defined __32MM0128GPM028__) || \
                  (defined __32MM0064GPM028__)
                #include "pins28.txt"
            //trouble
            #else
                #error "unknown PIC32MM in use, see Pins.hpp"
            #endif

            enum
IOMODE      : uint8_t {
            AIN = 0,
            IN = 1, INPU = 1<<3|1<<2|IN, INPD = 1<<4|IN, INL = 1<<2|IN,
            OUT = 2, OUTL = 1<<2|OUT
            };

Pins        (RPN, IOMODE = AIN);

            //r/w pins

            auto
pinval      () -> bool;

            auto
latval      () -> bool;

            auto
latval      (bool) -> void;

            auto
adcval      () -> uint16_t;

            auto
low         () -> void;

            auto
high        () -> void;

            auto
invert      () -> void;

            auto
on          () -> void;

            auto
off         () -> void;

            auto
ison        () -> bool;

            //pin modes

            auto
lowison     (bool) -> void;

            auto
digital_in  () -> void;

            auto
analog_in   () -> void;

            auto
digital_out () -> void;

            auto
odrain      (bool) -> void;

            auto
pullup      (bool) -> void;

            auto
pulldn      (bool) -> void;

            //icn

            auto
icn         (bool) -> void;

            auto
icn_rising  () -> void;

            auto
icn_falling () -> void;

            auto
icn_risefall () -> void;

            auto
icn_mismatch () -> void;

            auto
icn_flag    () -> bool;

            auto
icn_stat    () -> bool;

            auto
icn_flagclr () -> void;

            //pps

            enum
PPSIN       : uint8_t {
            //byte offset from RPINR1
            INT4 = 0*16|0,                          //R1
            ICM1 = 1*16|2, ICM2 = 1*16|3,           //R2
            ICM3 = 2*16|0,                          //R3
            //no RPINR4
            OCFA = 4*16|2, OCFB = 1*16|3,           //R5
            TCKIA = 5*16|0, TCKIB = 5*16|1,         //R6
            ICM5 = 6*16|0, ICM6 = 6*16|1,           //R7
            ICM7 = 6*16|2, ICM8 = 6*16|3,           //R7
            ICM9 = 7*16|0, U3RX = 7*16|3,           //R8
            U2RX = 8*16|2, U2CTS = 8*16|3,          //R9
            U3CTS = 9*16|3,                         //R10
            SDI2 = 10*16|0, SCK2IN = 10*16|1,       //R11
            SS2IN = 10*16|2,                        //R11
            CLCINA = 11*16|2, CLCINB = 11*16|3,     //R12
            PPSINOFF = 255
            };

            auto
pps_in      (PPSIN) -> void;

            enum
PPSOUT      : uint8_t {
            PPSLAT,
            C1OUT, C2OUT, C3OUT,
            U2TX, U2RTS, U3TX, U3RTS,
            SDO2, SCK2OUT, SS2OUT,
            OCM4, OCM5, OCM6, OCM7, OCM8, OCM9,
            CLC1OUT, CLC2OUT, CLC3OUT, CLC4OUT,
            };

            auto
pps_out     (PPSOUT) -> void;

            auto
an_num      () -> uint8_t;           //get adc #


            private:

            static auto
pps_do      (uint32_t, uint8_t) -> void;

            volatile uint32_t*  m_pt;       //base address
            const uint16_t      m_pn;       //pin mask
            bool                m_lowison;  //pin on val is low
            const uint8_t       m_rpn;      //RPn value
            uint8_t             m_ppsin;    //store ppsin peripheral
            const uint8_t       m_an;       //ANn value

};
