#pragma once

/*=============================================================================
 Peripheral Module Disable
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"

namespace Pmd {

    enum
    {
        PMDCON = 0xBF8035B0,
        PMD1_ADDR = 0xBF8035C0,
        PMDLOCK = 1<<11
    };

    //bit shift amount into 7 registers (PMD1-PMD7, 224 possible values)
    enum PMD
    {
        ADC = 0, VREF = 12, HLVD = 20,                          //PMD1
        CMP1 = 32, CMP2 = 33, CMP3= 34,                         //PMD2
        CLC1 = 56, CLC2 = 57, CLC3 = 58, CLC4 = 59,
        CCP1 = 72, CCP2 = 73, CCP3 = 74, CCP4 = 75,             //PMD3
        CCP5 = 76, CCP6 = 77, CCP7 = 78, CCP8 = 79, CCP9 = 80,
        T1 = 96, T2 = 97, T3 = 98,                              //PMD4
        U1 = 128, U2 = 129, U3 = 130,                           //PMD5
        SPI1 = 136, SPI2 = 137, SPI3 = 138,
        I2C1 = 144, I2C2 = 145, I2C3 = 146, USB = 152,
        RTCC = 160, REFO = 168,                                 //PMD6
        DMA = 196,                                              //PDM7
        END = 255 //for end of array value if using array
    };

    //private (not really)
    void _unlock( void )
    {
        Syskey::unlock();
        Reg::clr( PMDCON, PMDLOCK );
    }
    void _lock( void )
    {
        Reg::set( PMDCON, PMDLOCK );
        Syskey::lock();
    }
    //--private

    void off( PMD e )
    {
        _unlock();
        Reg::set( PMD1_ADDR + 16*(e/32), (1<<(e%32)) );
        _lock();
    }
    void on( PMD e )
    {
        _unlock();
        Reg::clr( PMD1_ADDR + 16*(e/32), (1<<(e%32)) );
        _lock();
    }

    //array of modules to disable/enable, END is end of array
    void off( PMD* e )
    {
        _unlock();
        for( ; *e != END; e++ ){
            Reg::set( PMD1_ADDR + 16*(*e/32), (1<<(*e%32)) );
        }
        _lock();
    }
    void on( PMD* e )
    {
        _unlock();
        for( ; *e != END; e++ ){
            Reg::clr( PMD1_ADDR + 16*(*e/32), (1<<(*e%32)) );
        }
        _lock();
    }

};


