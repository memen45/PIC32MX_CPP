#include "Syskey.hpp"

//syskey lock/unlock
//keep track of unlock count-
//inc unlock_count on lock(), dec unlock_count on unlock()
//unlock done when unlock_count is 0
//lock done when unlock_count is 0
static volatile uint8_t unlock_count;

void Syskey::lock(){
    bool irqstate = Irq::all_ison();                //get STATUS.IE
    ir.disable_all();
    //unlock_count only accessed with irq off
    if(unlock_count) unlock_count--;                //dec counter
    if(unlock_count == 0) r.val(SYSKEY, 0);         //if 0, lock
    //
    if(irqstate) ir.enable_all();                   //restore IE state
}

void Syskey::unlock(){
    bool irqstate = ir.all_ison();                  //get STATUS.IE
    ir.disable_all();
    bool dmasusp = r.anybit(DMACON, DMASUSP);       //get DMA suspend bit
    r.setbit(DMACON, DMASUSP);                      //DMA suspend
    //
    if(unlock_count == 0){                          //first time, unlock
        r.val(SYSKEY, MAGIC1);
        r.val(SYSKEY, MAGIC2);
    }
    unlock_count++;                                 //inc unlock_count
    //
    if(! dmasusp) r.clrbit(DMACON, DMASUSP);        //DMA resume
    if(irqstate) ir.enable_all();                   //restore IE state
}