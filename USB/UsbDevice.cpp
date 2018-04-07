#include "UsbDevice.hpp"
#include "Usb.hpp"
#include "UsbConfig.hpp"
#include "UsbBdt.hpp"
#include "UsbBuf.hpp"
#include "Irq.hpp"
#include "UsbEP.hpp"
#include "Delay.hpp"

#include <cstdio>

uint32_t UsbDevice::timer1ms = 0;
uint32_t UsbDevice::sof_count = 0;
UsbEP ep[UsbConfig::last_ep_num+1];
Pins vbus_pin(UsbConfig::vbus_pin_n);

//private
//=============================================================================
    void debug(const char* fmt, ...)
//=============================================================================
{
    if(not UsbConfig::debug_on) return;
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

//private
//=============================================================================
    bool detach()
//=============================================================================
{
    Usb usb; UsbBuf buf; Irq irq;

    bool wason = usb.power(usb.USBPWR);
    irq.on(irq.USB, false);             //usb irq off
    usb.power(usb.USBPWR, false);       //power off
    while(usb.power(usb.USBBUSY));      //wait for busy
    buf.init();                         //reclaim/clear buffers
    return wason;
}

//private
//=============================================================================
    void attach()
//=============================================================================
{
    Usb usb; Irq irq; UsbBdt bdt; UsbConfig cfg;

    UsbDevice::timer1ms = 0;            //reset 1ms timer
    UsbDevice::sof_count = 0;           //and sof count
    bdt.init();                         //clear bdt table
    //no writes to usb regs until powered on
    usb.power(usb.USBPWR, true);        //power on
    usb.bdt_addr((uint32_t)bdt.table);  //record address in usb bdt reg
    //init all ep (done with init function as need usb on first)
    for(uint8_t i = 0; i < UsbConfig::last_ep_num+1; i++) ep[i].init(i);
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
debug("%s:%d:%s(%s):\r\n", __FILE__, __LINE__, __func__,tf?"true":"false");

    bool wason = detach();
    //if no vbus pin voltage or tf=false (wanted only detach)
    if(not vbus_pin.ison() || not tf) return false;
    if(wason) Delay::wait_ms(100);      //delay if was previously on
    attach();
    return true;                        //true=attached
}

//=============================================================================
    bool        UsbDevice::cdc_tx       (uint8_t* buf, uint16_t count)
//=============================================================================
{
debug("%s:%d:%s(%08x, %d):\r\n", __FILE__, __LINE__, __func__,buf,count);
    return ep[2].xfer(ep[2].TX, buf, count);
}

//=============================================================================
    bool        UsbDevice::cdc_rx       (uint8_t* buf, uint16_t count)
//=============================================================================
{
debug("%s:%d:%s(%08x, %d):\r\n", __FILE__, __LINE__, __func__,buf,count);
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
    debug("%s:%d:%s():", __FILE__, __LINE__, __func__);
    debug("  1ms: %06d  sof: %06d  flags: %06x\r\n",
            UsbDevice::timer1ms,UsbDevice::sof_count,flags);
}

    //set 'normal' irq's, enable ep0 rx even/odd
    auto irqs_normal = [&](){
        usb.irqs(usb.IDLE bitor
                 usb.TRN bitor
                 usb.URST bitor
                 usb.T1MSEC bitor
                 usb.SOF);
    };

    if(flags bitand usb.T1MSEC){        //update 1ms timer
        UsbDevice::timer1ms++;
    }

    if(flags bitand usb.SOF){           //sof
        UsbDevice::sof_count++;
    }

    if(flags bitand usb.RESUME){
        irqs_normal();
    }

    if(flags bitand usb.URST){
        irqs_normal();                  //enable normal irq's
        return;                         //nothing more to do here
    }

    if(flags bitand usb.IDLE){          //idle (>3ms)
debug("\tIDLE\r\n");
        //set 'suspend' irq's
        usb.irqs(usb.RESUME bitor usb.URST bitor usb.T1MSEC);
    }

    if(flags bitand usb.TRN){           //token complete
        //although improbable, make sure an endpoint we use
        if(ustat <= UsbConfig::last_ep_num){
            ep[ustat>>2].trn_service(ustat bitand 3);
        }
        //else is not valid endpoint
    }

    //more flags to check- stall, etc.

    } while(irq.flag(irq.USB)); //check if irq set again before leaving isr

}


