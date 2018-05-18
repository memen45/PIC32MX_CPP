#include "UsbCentral.hpp"
#include "Usb.hpp"
#include "Irq.hpp"

#include <cstdio> //debug printf

//=============================================================================
    ISR(USB)
//=============================================================================
            {
            Usb usb; Irq irq;

            do {
                uint32_t flags = usb.flags();   //get all flags
                //get stat before trn flag cleared (which advances sie buffer)
                uint8_t stat = usb.stat();
                usb.flags_clr(flags);           //clear all flags we found
                irq.flag_clr(irq.USB);          //clear usb irq flag

                //DEBUG
                //ignore t1msec,sof flag for debug printf (too many)
                //print all flags and masked flags (enabled irqs)
                // if(flags bitand (compl (usb.T1MSEC bitor usb.SOF))){
                //      printf("\r\nISR  flags: %08x %08x\r\n",
                //          flags, flags bitand usb.irqs());
                // }

                //only need enabled irq flags, stat only valid if TRN set
                UsbCentral::service(flags bitand usb.irqs(), stat);

            } while(irq.flag(irq.USB)); //check if irq set again

            }
