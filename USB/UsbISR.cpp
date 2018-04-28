#include "UsbCentral.hpp"
#include "Usb.hpp"
#include "Irq.hpp"

#include <cstdio> //debug printf

//=============================================================================
    ISR(USB)
//=============================================================================
{
    Usb usb; Irq irq;

    do { //until no more usb irq flag

        uint32_t flags = usb.flags();
        uint8_t ustat = usb.stat();

//ignore t1msec,sof flag for debug printf
if(flags bitand (compl (usb.T1MSEC bitor usb.SOF))){
printf("\r\nISR flags: %08x", flags);
printf("  ustat: %02x",ustat);
}

        //clear all flags we found
        usb.flags_clr(flags);
        //clear usb irq flag
        irq.flag_clr(irq.USB);
        //need only flags with irq enabled
        flags and_eq usb.irqs();

if(flags bitand (compl (usb.T1MSEC bitor usb.SOF))){
printf("  flags: %08x\r\n",flags);
}


        //first do all TRN
        while(flags bitand usb.TRN){
            usb.irq(usb.URST, true);        //enable reset irq when any trn's
            UsbCentral::service(0, ustat);  //flags=0, ustat != 0xFF
            flags and_eq (compl usb.TRN);   //to break this while loop

            if(usb.flag(usb.TRN)){          //any more?
                ustat = usb.stat();         //yes, get ustat again
                usb.flags_clr(usb.TRN);     //clear usb flag
                flags or_eq usb.TRN;        //to keep inside this while loop
            }
        }

        UsbCentral::service(flags, 0xFF);   //now do other flags, ustat=0xFF


    } while(irq.flag(irq.USB)); //check if irq set again before leaving isr

}

