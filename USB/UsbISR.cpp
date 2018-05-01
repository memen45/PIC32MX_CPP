#include "UsbCentral.hpp"
#include "Usb.hpp"
#include "Irq.hpp"

#include <cstdio> //debug printf

//=============================================================================
// get all usb module irq flags except for TRN
// clear all flags that were set, clear usb irq flag
// mask off flags not needed (irq not enabled)
// check TRN flag and process if set (UsbCentral::service), until sie empty
// process all other flags (UsbCentral::service)
// repeat until no more usb irq's
//=============================================================================
    ISR(USB)
//=============================================================================
{
    Usb usb; Irq irq;

    do { //until no more usb irq flag

        uint32_t flags = usb.flags();       //get all flags
        flags and_eq (compl usb.TRN);       //mask trn, will handle below

//ignore t1msec,sof flag for debug printf
if(flags bitand (compl (usb.T1MSEC bitor usb.SOF))){
printf("\r\nISR  all flags: %08x", flags);
}
        usb.flags_clr(flags);               //clear all flags we found
        irq.flag_clr(irq.USB);              //clear usb irq flag
        flags and_eq usb.irqs();            //need only flags with irq enabled

//now show masked flags
if(flags bitand (compl (usb.T1MSEC bitor usb.SOF))){
printf("  flags: %08x\r\n",flags);
}

        while(usb.flag(usb.TRN)){           //first do all TRN
            usb.irq(usb.URST, true);        //enable reset irq when any trn's
            UsbCentral::service(0, usb.stat());  //flags=0, ustat != 0xFF
            usb.flags_clr(usb.TRN);         //clear trn flag after getting stat
        }

        UsbCentral::service(flags, 0xFF);   //now do other flags, ustat=0xFF


    } while(irq.flag(irq.USB)); //check if irq set again before leaving isr

}
