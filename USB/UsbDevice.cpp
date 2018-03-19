#include "UsbDevice.hpp"
#include "Usb.hpp"
#include "UsbConfig.hpp"
#include "UsbBdt.hpp"
#include "UsbBuf.hpp"
#include "Irq.hpp"


uint32_t UsbDevice::timer1ms = 0;
uint32_t UsbDevice::sof_count = 0;

//private
//=============================================================================
void detach()
//=============================================================================
{
    Usb usb; UsbBdt bdt; UsbBuf buf; Irq irq;

    irq.on(irq.USB, false);             //usb irq off
    usb.power(usb.USBPWR, false);       //power off
    while(usb.power(usb.USBBUSY));      //wait for busy
    bdt.table = {0};                    //clear bdt table
    buf.init();                         //reclaim/clear buffers
}

//private
//=============================================================================
void attach()
//=============================================================================
{
    Usb usb; Irq irq; UsbBdt bdt; UsbConfig cfg;

    timer1ms = 0;                       //reset 1ms timer
    sof_count = 0;                      //and sof count
    //get ep0 buffers, set bdt table addresses
    bdt.bufaddr[0], buf.get64());       //ep0 rx even
    bdt.bufaddr[1], buf.get64());       //ep0 rx odd
    bdt.bufaddr[2], buf.get64());       //ep0 tx even
    bdt.bufaddr[3], buf.get64());       //ep0 tx odd
    //no writes to usb regs until powered on
    usb.power(usb.USBPWR, true);        //power on
    usb.bdt_addr(bdt.table);            //record address in usb bdt reg
    usb.epcontrol(0, usb.EPRXEN|usb.EPTXEN|usb.EPHSHK); //enable endpoint 0
    usb.irqs(usb.URST|usb.T1MSEC);      //enable some irqs
    irq.init(irq.USB, cfg.usb_irq_pri, cfg.usb_irq_subpri, true); //usb irq on
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
    detach();
    //if no vbus pin voltage or tf=false (wanted only detach)
    if(not usb.vbus_pin.ison() || not tf) return false;
    attach();
    return true;                        //true=attached
}


//=============================================================================
    ISR(USB)
//=============================================================================
{
    Usb usb; Irq irq; UsbBdt bdt;

    uint32_t flags = usb.flags();       //get all flags

    //get stat (stat values 0-63, only valid if trn set)
    //sie fifo advances when trn cleared, so get a copy before trn cleared
    //(if trn not set, the flag clearing below will not clear, so no advance)
    uint8_t stat = usb.stat();

    usb.flags_clr(flags);               //clear all flags we found
    irq.flag_clr(irq.USB);              //clear usb irq flag
    flags and_eq usb.irqs();            //need only flags with irq enabled

    //all flags cleared now, and only interested in flags where irq
    //was also enabled, we also have stat if trn was set

    //enable ep0 rx (even=false|0, odd=true|1)
    auto enable_ep0rx = [&](bool eo){
        bdt.table[eo].ctrl.count = 64;
        bdt.table[eo].ctrl.data01 = 0;
        bdt.table[eo].ctrl.bstall = 1;
        bdt.table[eo].ctrl.uown = 1;
    }

    //set 'normal' irq's, enable ep0 rx even/odd
    auto irqs_normal = [&](){
        usb.irqs(usb.IDLE bitor
                 usb.TRN bitor
                 usb.URST bitor
                 usb.T1MSEC bitor
                 usb.SOF);
        enable_ep0rx(0);
        enable_ep0rx(1);
    }

    //set 'suspend' irq's
    auto irqs_suspend = [&](){
        usb.irqs(usb.RESUME bitor
                 usb.URST bitor
                 usb.T1MSEC bitor);
    }


    if(flags bitand usb.T1MSEC){        //update 1ms timer
        timer1ms++;
    }

    if(flags bitand usb.SOF){           //sof
        sof_count++;
    }

    if(flags bitand usb.RESUME){
        irqs_normal();
    }

    if(flags bitand usb.URST){
        //if idle or resume irq was on, was not just attached
        //so is a reset when irq's were 'normal', so reinit
        if(usb.irq(us.RESUME) or usb.irq(usb.IDLE)) init(true);
        else irqs_normal();             //just attached + reset
        return;                         //nothing more to do here
    }

    if(flags bitand usb.IDLE){          //idle (>3ms)
        irqs_suspend();
    }

    if(flags bitand usb.TRN){           //token complete
        //if ep0 rx, arm the next rx buffer
        //stat-> ep0rxeven = 0, ep0rxodd=1
        if(stat <= 1) enable_ep0rx(stat xor 1);
        //call something with stat (index into table buffer)
        //some_trn_func(stat)
    }
}
