#include "UsbDevice.hpp"
#include "Usb.hpp"
#include "UsbBuf.hpp"
#include "Irq.hpp"
#include "UsbEP.hpp"
#include "Delay.hpp"
#include "UsbCentral.hpp"

#include <cstdio>

uint32_t UsbDevice::timer1ms = 0;
uint32_t UsbDevice::sof_count = 0;
UsbEP ep[UsbCentral::last_ep_num+1];

//private
//=============================================================================
    void detach()
//=============================================================================
{
    Usb usb; UsbBuf buf; Irq irq;

    irq.on(irq.USB, false);             //usb irq off
    usb.power(usb.USBPWR, false);       //power off
    buf.init();                         //reclaim/clear buffers
}

//private
//=============================================================================
    void attach()
//=============================================================================
{
    Usb usb; Irq irq;

    UsbDevice::timer1ms = 0;            //reset 1ms timer
    UsbDevice::sof_count = 0;           //and sof count
    //no writes to usb regs until powered on
    while(usb.power(usb.USBBUSY));      //wait for busy
    usb.power(usb.USBPWR, true);        //power on (also inits bdt table)
    //init all ep (done with init function as need usb on first)
    for(uint8_t i = 0; i <= UsbCentral::last_ep_num; i++) ep[i].init(i);

//TODO
//init ep0 here
//call UsbCentral::service(usb.URST, 0) , which will init
//current config (endpoints, whatever it needs done
//passing reset flag will indicate it needs to reset itself


    usb.irqs(usb.SOF|usb.T1MSEC|usb.TRN|usb.IDLE);   //enable some irqs
    irq.init(irq.USB, Usb::usb_irq_pri, Usb::usb_irq_subpri, true); //usb irq on
    usb.control(usb.USBEN, true);       //enable usb module
    irq.global(true);                   //global irq's on if not already
}

//init usb (or reinit, or detach) true=init/reinit, false=detach
//return true if attached, false if detached
//global irq's enabled if attached
//=============================================================================
    bool        UsbDevice::init         (bool tf)
//=============================================================================
{
printf("\r\n\r\nUsbDevice::init(%d)\r\n",tf);
    detach();
    //if no vbus pin voltage or tf=false (wanted only detach)
    if(not Usb::vbus_ison() || not tf){
printf("false\r\n");
        return false;
    }
    attach();
    return true;                        //true=attached
}

//=============================================================================
    bool        UsbDevice::cdc_tx       (uint8_t* buf, uint16_t count)
//=============================================================================
{
    return ep[2].xfer(ep[2].TX, buf, count);
}

//=============================================================================
    bool        UsbDevice::cdc_rx       (uint8_t* buf, uint16_t count)
//=============================================================================
{
    return ep[2].xfer(ep[2].RX, buf, count);
}





//=============================================================================
    ISR(USB)
//=============================================================================
{
    Usb usb; Irq irq;

    do { //until no more usb irq flag

    uint32_t flags = usb.flags();       //get all flags

    //get stat (stat values 0-63, only valid if trn set)
    //sie fifo advances when trn cleared, so get a copy before trn cleared
    //(if trn not set, the flag clearing below will not clear, so no advance)
    uint8_t ustat = usb.stat();

    usb.flags_clr(flags);               //clear all flags we found
    irq.flag_clr(irq.USB);              //clear usb irq flag
    flags and_eq usb.irqs();            //need only flags with irq enabled

    //all flags cleared now, and only interested in flags where irq
    //was also enabled, we also have stat if trn was set

if(flags bitand compl (usb.T1MSEC bitor usb.SOF)){
    printf("\r\nISR> frame: %d  flags: %08x\r\n",Usb::frame(),flags);
}

    if(flags bitand usb.T1MSEC){
        UsbDevice::timer1ms++;
    }
    if(flags bitand usb.SOF){
        UsbDevice::sof_count++;
    }
    if(flags bitand usb.RESUME){
        usb.irq(usb.RESUME, false);
    }

    if(flags bitand usb.IDLE){          //idle (>3ms)
        //set 'suspend' irq's
        usb.irq(usb.RESUME, true);
    }

    while(flags bitand usb.TRN){        //token complete
        usb.irq(usb.URST, true);        //enable reset irq after any token

//TODO
//change
//check for ep0, handle in this file
//else send to UsbCentral::service

        //although improbable, make sure an endpoint we use
        uint8_t n = ustat>>2;
        if(n <= UsbCentral::last_ep_num){
            //ep[ustat>>2].trn_service(ustat bitand 3);
            ep[n].service(ustat bitand 3);
        }
        //else is not valid endpoint
        if(usb.flag(usb.TRN)){
            ustat = usb.stat();
            usb.flags_clr(usb.TRN);
            flags = usb.TRN;
        } else break;
    }

    if(flags bitand usb.URST){
        UsbDevice::init(true);
        return;
    }
    //more flags to check- stall, etc.

    } while(irq.flag(irq.USB)); //check if irq set again before leaving isr

}


