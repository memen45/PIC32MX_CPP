#pragma once

#include "Irq.hpp"
#include "Reg.hpp"

namespace Syskey {

    enum
    {
        SYSKEY_ADDR = 0xBF803670,
        DMACON = 0xBF808900,
        SUSPEND = 1<<12
    };

    //syskey lock/unlock
    //keep track of unlock count- do not lock until unlock_count is 0
    //so we don't get locks while unlock in use elsewhere
    // code A - unlock; do something; lock;
    // irq B - unlock; do something; lock;
    static uint8_t unlock_count;

    void lock( void ){
        if( unlock_count ) unlock_count--;
        if( unlock_count ) return;
        Reg::val( SYSKEY_ADDR, 0 );
    }

    void unlock( void )
    {
        bool irqstate = Irq::all_ison();                //get STATUS.IE
        Irq::disable_all();
        bool dmasusp = Reg::is_set( DMACON, SUSPEND );  //get DMA suspend bit
        Reg::set( DMACON, SUSPEND );                    //DMA suspend
        if( ! unlock_count ){                           //first time, unlock
            Reg::val( SYSKEY_ADDR, 0 );                 //not sure why
            Reg::val( SYSKEY_ADDR, 0xAA996655 );
            Reg::val( SYSKEY_ADDR, 0x556699AA );
        }
        unlock_count++;                                 //inc unlock_count
        if( ! dmasusp ) Reg::clr( DMACON, SUSPEND );    //DMA resume
        if( irqstate ) Irq::enable_all();               //restore IE state
    }

};