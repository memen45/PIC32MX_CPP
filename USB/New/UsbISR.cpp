#include "UsbCentral.hpp"
#include "Usb.hpp"
#include "Irq.hpp"

#include <cstdio> //debug printf

//=============================================================================
    ISR(USB)
//=============================================================================
{
    Usb usb; Irq irq;
    static uint8_t ustat[5];

    do { //until no more usb irq flag

        //get all flags, mask out TRN- will handle separately
        uint32_t flags = usb.flags() bitand compl usb.TRN;
        
        //get all ustat from sie buffer (up to 4)
        //0xFF = not a valid stat, use to mark the end of array
        uint8_t i = 0
        for(; i < 4 and usb.flag(usb.TRN); i++){
            ustat[i] = usb.stat();
            usb.flags_clr(usb.TRN);
            usb.irq(usb.URST, true); //enable reset irq when any trn's
        } 
        ustat[i] = 0xFF; //mark end of array       

        //clear all flags we found (except TRN, which was already handled)
        usb.flags_clr(flags);
        //clear usb irq flag        
        irq.flag_clr(irq.USB); 
        //need only flags with irq enabled        
        flags and_eq usb.irqs();            

        //all flags cleared now, and only interested in flags where irq
        //was also enabled, we also have all sie buffer (if any)
        //TRN flag will not be in flags, so callee just needs to check 
        //ustat array (valid TRN's until find 0xFF in array)
       
        UsbCentral::service(flags, ustat);
    
    } while(irq.flag(irq.USB)); //check if irq set again before leaving isr

}

