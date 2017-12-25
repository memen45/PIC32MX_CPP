#include "Syskey.hpp"

//syskey lock/unlock
//keep track of unlock count- do not lock until unlock_count is 0
//so we don't get locks while unlock in use elsewhere
// code A - unlock; (irq in here) do something; lock;
// irq B - unlock; do something; lock; <- A is now locked out
//with unlock_count-
// code A- unlock (count=1); (irq in here) do something; lock(count=0,lock);
// irq B- unlock(count=2); do something; lock(count=1,no lock);
static uint8_t unlock_count;

//not sure if irq disable required, but probably is as this possibly
//could be called in irq
void Syskey::lock(){
    bool irqstate = Irq::all_ison();                //get STATUS.IE
    ir.disable_all();
    //
    if(unlock_count) unlock_count--;                //dec counter
    if(unlock_count == 0) r.val(SYSKEY_ADDR, 0);    //if 0, lock
    //
    if(irqstate) ir.enable_all();                   //restore IE state
}

void Syskey::unlock(){
    bool irqstate = ir.all_ison();                  //get STATUS.IE
    ir.disable_all();
    bool dmasusp = r.is_set(DMACON, DMASUSP);       //get DMA suspend bit
    r.set(DMACON, DMASUSP);                         //DMA suspend
    //
    if(! unlock_count){                             //first time, unlock
        r.val(SYSKEY_ADDR, MAGIC1);
        r.val(SYSKEY_ADDR, MAGIC2);
    }
    unlock_count++;                                 //inc unlock_count
    //
    if(! dmasusp) r.clr(DMACON, DMASUSP);           //DMA resume
    if(irqstate) ir.enable_all();                   //restore IE state
}