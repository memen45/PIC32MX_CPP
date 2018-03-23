#include "UsbDevice.hpp"
#include "Usb.hpp"
#include "UsbConfig.hpp"
#include "UsbBdt.hpp"
#include "UsbBuf.hpp"
#include "Irq.hpp"


uint32_t UsbDevice::timer1ms = 0;
uint32_t UsbDevice::sof_count = 0;
UsbEP ep[UsbConfig::last_ep_num+1];

//private
//=============================================================================
    void debug(...)
//=============================================================================
{
    if(not UsbConfig::debug_on) return;
    printf("USB: %s:%d:%s(): ", __FILE__, __LINE__, __func__);
    printf( __VA_ARGS__ );
    printf("\r\n");
}

//private
//=============================================================================
    void detach()
//=============================================================================
{
    Usb usb; UsbBdt bdt; UsbBuf buf; Irq irq;

    irq.on(irq.USB, false);             //usb irq off
    usb.power(usb.USBPWR, false);       //power off
    while(usb.power(usb.USBBUSY));      //wait for busy
    buf.init();                         //reclaim/clear buffers
}

//private
//=============================================================================
    void attach()
//=============================================================================
{
    Usb usb; Irq irq; UsbBdt bdt; UsbConfig cfg; UsbBuf ubuf;

    timer1ms = 0;                       //reset 1ms timer
    sof_count = 0;                      //and sof count
    bdt.init();                         //clear table
    for(uint8_t i = 0; i < UsbConfig::last_ep_num+1; i++) ep[i].init(i);
    ep[0].setbuf(ubuf.get64(), ep[0].RX, ep[0].EVEN, 64);
    ep[0].setbuf(ubuf.get64(), ep[0].RX, ep[0].ODD, 64);
    ep[0].setbuf(ubuf.get64(), ep[0].TX, ep[0].EVEN, 64);
    ep[0].setbuf(ubuf.get64(), ep[0].TX, ep[0].ODD, 64);
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

    //set 'normal' irq's, enable ep0 rx even/odd
    auto irqs_normal = [&](){
        usb.irqs(usb.IDLE bitor
                 usb.TRN bitor
                 usb.URST bitor
                 usb.T1MSEC bitor
                 usb.SOF);
        enable_ep0rx();
        enable_ep0rx();
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
        //set 'suspend' irq's
        usb.irqs(usb.RESUME bitor
                 usb.URST bitor
                 usb.T1MSEC bitor);
    }

    if(flags bitand usb.TRN){           //token complete
        //although improbable, make sure an endpoint we use
        if(stat < UsbBdt::bdt_table_siz){
            ep[ustat>>2].trn_service(ustat bitand 3);
        }
        //else is not valid endpoint
    }

    //more flags to check- stall, etc.

    } while(irq.flag(irq.USB); //check if irq set again before leaving isr
}


