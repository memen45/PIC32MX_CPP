//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "Reg.hpp"
#include "UsbConfig.hpp"
#include "Pins.hpp"

//registers - all registers use only first 8bits
enum {
    U1OTGIR = 0xBF808440,   //no SET, INV - all bits write-1-to-clear
    U1OTGIE = 0xBF808450,
    U1OTGSTAT = 0xBF808460, //no SET, INV, CLR
    U1OTGCON = 0xBF808470,
    U1PWRC = 0xBF808480,
    U1IR = 0xBF808600,//no SET, INV
    U1IE = 0xBF808610,
    U1EIR = 0xBF808620,//no SET, INV
    U1EIE = 0xBF808630,
    U1STAT = 0xBF808640,//no SET, INV, CLR
        ENDPT_SHIFT = 4, ENDPT_MASK = 15,
        DIR = 3,
        PPBI = 2,
    U1CON = 0xBF808650,
    U1ADDR = 0xBF808660,
        LSEN = 7,
    U1BDTP1 = 0xBF808670,
    U1FRML = 0xBF808680, //no SET, INV, CLR
    U1FRMH = 0xBF808690, //no SET, INV, CLR
    U1TOK = 0xBF8086A0,
    U1SOF = 0xBF8086B0,
    U1BDTP2 = 0xBF8086C0,
    U1BDTP3 = 0xBF8086D0,
    U1CNFG1 = 0xBF8086E0,
    U1EP0 = 0xBF808700, U1EP_SPACING = 0x10
};

volatile Usb::BDT_ENTRY Usb::BDT[16] = {0};
Pins vbus_pin(UsbConfig::vbus_pin_n, Pins::INPD);

//U1OTGIR
//=============================================================================
    uint8_t     Usb::otg_flags          ()
//=============================================================================
{
    return Reg::val8(U1OTGIR);
}

//=============================================================================
    bool        Usb::otg_flag           (FLAGSOTG e)
//=============================================================================
{
    return Reg::anybit(U1OTGIR, e);
}

//=============================================================================
    void        Usb::otg_flags_clr      ()
//=============================================================================
{
    Reg::val8(U1OTGIR, 255);
}

//=============================================================================
    void        Usb::otg_flags_clr      (uint8_t v)
//=============================================================================
{
    Reg::val8(U1OTGIR, v);
}

//=============================================================================
    void        Usb::otg_flag_clr       (FLAGSOTG e)
//=============================================================================
{
    Reg::val8(U1OTGIR, e);
}

//U1OTGIE
//=============================================================================
    uint8_t     Usb::otg_irqs           ()
//=============================================================================
{
    return Reg::val8(U1OTGIR);
}

//=============================================================================
    bool        Usb::otg_irq            (FLAGSOTG e)
//=============================================================================
{
    return Reg::anybit(U1OTGIR, e);
}

//=============================================================================
    void        Usb::otg_irqs_clr       ()
//=============================================================================
{
    Reg::val8(U1OTGIR, 255);
}

//=============================================================================
    void        Usb::otg_irqs_clr       (uint8_t v)
//=============================================================================
{
    Reg::val8(U1OTGIR, v);
}

//=============================================================================
    void        Usb::otg_irq_clr        (FLAGSOTG e)
//=============================================================================
{
    Reg::val8(U1OTGIR, e);
}

//U1OTGSTAT
//=============================================================================
    uint8_t     Usb::otg_stat           ()
//=============================================================================
{
    return Reg::val8(U1OTGSTAT);
}

//=============================================================================
    bool        Usb::otg_stat           (FLAGSOTG e)
//=============================================================================
{
    return Reg::anybit(U1OTGSTAT, e);
}

//U1OTGCON
//=============================================================================
    void        Usb::otg                (OTG e, bool tf)
//=============================================================================
{
    Reg::setbit(U1OTGCON, e, tf);
}

//=============================================================================
    void        Usb::otg                (uint8_t v)
//=============================================================================
{
    Reg::val8(U1OTGCON, v);
}

//U1PWRC
//=============================================================================
    bool        Usb::power              (POWER e)
//=============================================================================
{
    return Reg::anybit(U1PWRC, e);
}

//=============================================================================
    void        Usb::power              (POWER e, bool tf)
//=============================================================================
{
    Reg::setbit(U1PWRC, e, tf);
}

//U1IR
//=============================================================================
    uint8_t     Usb::flags              ()
//=============================================================================
{
    return Reg::val8(U1IR);
}

//=============================================================================
    bool        Usb::flag               (FLAGS e)
//=============================================================================
{
    return Reg::anybit(U1IR, e);
}

//=============================================================================
    void        Usb::flags_clr          (uint8_t v)
//=============================================================================
{
    Reg::val8(U1IR, v);
}

//U1IE
//=============================================================================
    uint8_t     Usb::irqs               ()
//=============================================================================
{
    return Reg::val8(U1IE);
}

//=============================================================================
    bool        Usb::irq                (FLAGS e)
//=============================================================================
{
    return Reg::anybit(U1IE, e);
}

//=============================================================================
    void        Usb::irqs               (uint8_t v)
//=============================================================================
{
    Reg::val8(U1IE, v);
}

//=============================================================================
    void        Usb::irq                (FLAGS e, bool tf)
//=============================================================================
{
    Reg::setbit(U1IE, e, tf);
}

//U1EIR
//=============================================================================
    uint8_t     Usb::eflags             ()
//=============================================================================
{
    return Reg::val8(U1EIR);
}

//=============================================================================
    bool        Usb::eflag              (EFLAGS e)
//=============================================================================
{
    return Reg::anybit(U1EIR, e);
}

//=============================================================================
    void        Usb::eflags_clr         (uint8_t v)
//=============================================================================
{
    Reg::val8(U1EIR, v);
}

//U1EIE
//=============================================================================
    uint8_t     Usb::eirqs              ()
//=============================================================================
{
    return Reg::val8(U1EIE);
}

//=============================================================================
    bool        Usb::eirq               (EFLAGS e)
//=============================================================================
{
    return Reg::anybit(U1EIE, e);
}

//=============================================================================
    void        Usb::eirqs              (uint8_t v)
//=============================================================================
{
    Reg::val8(U1EIE, v);
}

//=============================================================================
    void        Usb::eirq               (EFLAGS e, bool tf)
//=============================================================================
{
    Reg::setbit(U1EIE, e, tf);
}

//U1STAT
//=============================================================================
    uint8_t     Usb::stat_endp          ()
//=============================================================================
{
    return (Reg::val8(U1STAT) >> ENDPT_SHIFT) & ENDPT_MASK;
}

//=============================================================================
    bool        Usb::stat_dir           ()
//=============================================================================
{
    return Reg::anybit(U1STAT, 1<<DIR);
}

//=============================================================================
    bool        Usb::stat_ppbi          ()
//=============================================================================
{
    return Reg::anybit(U1STAT, 1<<PPBI);
}

//U1CON
//=============================================================================
    bool        Usb::control            (CONTROL e)
//=============================================================================
{
    return Reg::anybit(U1CON, e);
}

//=============================================================================
    void        Usb::control            (CONTROL e, bool tf)
//=============================================================================
{
    Reg::setbit(U1CON, e, tf);
}

//=============================================================================
    void        Usb::control            (uint8_t v)
//=============================================================================
{
    Reg::val8(U1CON, v);
}

//U1ADDR
//=============================================================================
    void        Usb::low_speed          (bool tf)
//=============================================================================
{
    Reg::setbit(U1ADDR, 1<<LSEN, tf);
}

//=============================================================================
    uint8_t     Usb::dev_addr           ()
//=============================================================================
{
    return Reg::val8(U1ADDR) bitand 127;
}

//=============================================================================
    void        Usb::dev_addr           (uint8_t v)
//=============================================================================
{
    Reg::clrbit(U1ADDR, 127);
    Reg::setbit(U1ADDR, v bitand 127);
}

//U1FRML,H
//=============================================================================
    uint16_t    Usb::frame              ()
//=============================================================================
{
    return (Reg::val8(U1FRMH)<<8) | Reg::val8(U1FRML);
}

//U1TOK
//=============================================================================
    uint8_t     Usb::tok_pid            (TOKPID e)
//=============================================================================
{
    Reg::val8(U1TOK, e<<4);
}

//=============================================================================
    void        Usb::tok_ep             (uint8_t v)
//=============================================================================
{
    Reg::clrbit(U1TOK, 15);
    Reg::setbit(U1TOK, v bitand 15);
}

//U1SOF
//=============================================================================
    void        Usb::sof_cnt            (SOFVALS e)
//=============================================================================
{
    Reg::val8(U1SOF, e);
}

//U1BDTP1,2,3
//=============================================================================
    void        Usb::bdt_addr           (uint32_t v)
//=============================================================================
{
    v = Reg::k2phys(v); //physical address
    Reg::val8(U1BDTP1, v>>8); //512byte aligned (bit0 of this reg unused)
    Reg::val8(U1BDTP2, v>>16);
    Reg::val8(U1BDTP3, v>>24);
}

//=============================================================================
    uint32_t    Usb::bdt_addr           ()
//=============================================================================
{
    uint32_t ret =
        Reg::val8(U1BDTP3)<<24 | Reg::val8(U1BDTP2)<<16 | Reg::val8(U1BDTP1)<<8;
    return  Reg::p2kseg0(ret); //virtual address i kseg0
}

//U1CNFG1
//=============================================================================
    void        Usb::config             (CONFIG e, bool tf)
//=============================================================================
{
    Reg::setbit(U1CNFG1, e, tf);
}

//=============================================================================
    bool        Usb::config             (CONFIG e)
//=============================================================================
{
    return Reg::anybit(U1CNFG1, e);
}

//=============================================================================
    void        Usb::config             (uint8_t v)
//=============================================================================
{
    Reg::val8(U1CNFG1, v);
}

//U1EP0-15
//=============================================================================
    void        Usb::epcontrol          (uint8_t n, EPCTRL e, bool tf)
//=============================================================================
{
    n and_eq 15;
    Reg::setbit(U1EP0+(n * U1EP_SPACING), e, tf);
}

//=============================================================================
    bool        Usb::epcontrol          (uint8_t n, EPCTRL e)
//=============================================================================
{
    n and_eq 15;
    Reg::anybit(U1EP0+(n * U1EP_SPACING), e);
}

//=============================================================================
    void        Usb::epcontrol          (uint8_t n, uint8_t v)
//=============================================================================
{
    n and_eq 15;
    Reg::val8(U1EP0+(n * U1EP_SPACING), v);
}

//=============================================================================
    uint8_t     Usb::epcontrol          (uint8_t n)
//=============================================================================
{
    n and_eq 15;
    return Reg::val8(U1EP0+(n * U1EP_SPACING));
}

//misc
//init usb (or reinit)
//=============================================================================
    bool        Usb::init               ()
//=============================================================================
{
    power(USBPWR, false);       //power off
    while(power(USBBUSY));      //wait for busy
    UsbBdt::init();             //clear bdt table
    if(not vbus_pin.ison()) return false; //no vbus present
    power(USBPWR, true);        //power on
    bdt_addr(UsbBdt::table);    //record address in usb bdt reg
    return true;
}

//add irq here
#include "Irq.hpp"
ISR(USB){
    USBDeviceTasks(); //whatever function name this will be
}


#if 0
//power down usb module, usb irq's off
//=============================================================================
    void        Usb::detach             (void)
//=============================================================================
{
    Usb u; Irq ir;
    u.control(u.USBEN, false);  //disable usb module, detach from bus
    ir.on(ir.USB, false);       //all usb irqs off
    while(u.power(u.BUSY));     //wait for busy bit to clear first
    u.power(u.USBPWR, false);   //usb module off
    u.flags_clr(u.ALLFLAGS);    //clear all flags
    u.flags_clr(u.ALLEFLAGS);   //clear all eflags

    u.state = u.DETACHED;

    //wait 100ms+ before attach again (if we are cause of reset/detach and
    //was previously in >= DEFAULT state)
    //if was a usb reset that caused the attach function to run (and this)
    //then the usb host already is in reset and a quick attach will cause no
    //problems (I think that's correct)
}


//called from init (which first calls detach)
//called from isr (when >=DEFAULT and reset flag)
//=============================================================================
    void            Usb::attach         (void)
//=============================================================================
{
    Usb u; UsbBuf ubuf; Irq ir;

    detach();                       //all off (regs will reset from off->on)
    u.power(u.USBPWR, true);        //usb on (all regs now reset)

    u.bdt_addr(UsbEndpt::bdt_addr());//set bdt address
    ubuf.reinit();                  //clear all buffers, clear in-use flag

    for(auto& i : UsbConfig::endpoints) i.reinit();//init (reinit) each UsbEndpt
    UsbConfig::endpoints[0].on(true);   //and enable endpoint 0

    //enable irqs
    u.irqs(u.RESET); // only looking for reset first
    //u.irqs(u.STALL|u.IDLE|u.TOKEN|u.SOF|u.ERROR|u.RESET);
    //u.eirqs(u.BSTUFF|u.BTMOUT|u.DATSIZ|u.CRC16|u.CRC5|u.PID);

    u.state = u.ATTACHED;
    ir.init(ir.USB, UsbConfig::usb_irq_pri, UsbConfig::usb_irq_subpri, true);

    u.control(u.USBEN, true);       //enable usb

    //isr takes over
}


//call once to init usb
//=============================================================================
    void            Usb::init           ()
 //=============================================================================
 {
printf("USB::init:\r\n");
    UsbConfig::vbus_pin.digital_in();   //vbus/rb6 to input
//    UsbConfig::vbus_pin.pulldn(true);   //pulldown when no vbus keeps pin low
    attach();                           //init all things
 }



//=============================================================================
    ISR(USB)
//=============================================================================
{
printf("USB ISR> ");
    Usb u; Irq ir;                  // u. for Usb:: , ir. for Irq::
    static Usb::STATE last_state;   //keep track of usb state for resumes

    Usb::flags_t flags;
    Usb::eflags_t eflags;
    Usb::stat_t stat;

    flags.all = u.flags();          //get all usb specific irq flags
    eflags.all = u.eflags();        //get all usb specific irq error flags
    stat.all = u.stat();            //get stat reg BEFORE flags cleared

    u.flags_clr(flags.all);         //clear only what we got (1=clear)
    u.eflags_clr(eflags.all);       //clear only what we got (1=clear)
    ir.flag_clr(ir.USB);            //clear usb irq flag

    flags.all and_eq u.irqs();      //mask off irq's not enabled
    eflags.all and_eq u.eirqs();    //mask off error irq's not enabled

    //shouldn't happen, but check anyway to make sure we stay inside
    //our ep array
    if(stat.endpt > UsbConfig::last_endp) return;

printf("flags: %u  eflags: %u  stat: %u  state: %u  endpoint: %u\r\n",
        flags.all, eflags.all, stat.all, u.state, stat.endpt);

    //nested function, check if need to suspend (idle detected >3ms)
    //if go SUSPENDED, all flags will be cleared and only a resume
    //or reset will get us going again (we were idle for a while so
    //nothing going on, so safe to clear all flags)
    auto _is_idle = [ & ](){
        if (flags.all != u.IDLE) return false;
        last_state = u.state;               //save state for resume
        u.state = u.SUSPENDED;              //now in SUSPENDED state
        u.flags_clr(u.ALLFLAGS);            //clear flags
        u.eflags_clr(u.ALLEFLAGS);          //clear flags
        u.irqs(u.RESUME|u.RESET);           //only resume or reset resumes
        //do suspend- whatever is needed
        return true;
    };

    //nested function, check if reset flag, if so start over by calling
    //attach() (only called from >=DEFAULT state)
    auto _is_reset = [ & ](){
        if (flags.reset == 0) return false;
        u.attach();
        last_state = u.ATTACHED;
        return true;
    };

    //nested function, set to 'default' usb interrupts from
    //idle+reset or resume+reset, or reset only
    //clear all flags first
    auto _default = [ & ](){
        u.flags_clr(u.ALLFLAGS);            //clear flags
        u.eflags_clr(u.ALLEFLAGS);          //clear flags
        u.irqs(u.STALL|u.IDLE|u.TOKEN|/*u.SOF|*/u.ERROR|u.RESET);
        u.eirqs(u.BSTUFF|u.BTMOUT|u.DATSIZ|u.CRC16|u.CRC5|u.PID);
    };

    //usb states-
    //DETACHED -we never see here, usb peripheral off
    //ATTACHED, POWERED, DEFAULT, ADDRESS, CONFIGURED, SUSPENDED
    switch(u.state){
        case u.DETACHED:                    //can't get here- usb irq is off
            return;
        case u.ATTACHED:                    //only reset irq is active
            if(not UsbConfig::vbus_pin.ison()) return; //no vbus, we should not be here
            u.state = u.POWERED;            //vbus ok, now go to POWERED state
            u.flags_clr(u.IDLE);            //clear idle flag before enabling
            u.irqs(u.IDLE|u.RESET);         //now add idle irq
            //fall through
        case u.POWERED:                     //reset and idle irq's active
            if(_is_idle()) return;          //if idle, switch state and return
            if(flags.reset == 0) return;    //need to see reset before we
            u.state = u.DEFAULT;            //go to DEFAULT state
            _default();                     //was reset, so enable default irq's
            return;

        case u.DEFAULT:                     //device adddress is 0
        case u.ADDRESS:                     //other code sets this state
        case u.CONFIGURED:                  //other code sets this state
            if(_is_idle()) return;          //if idle, switch state and return
            break;                          //only these 3 states continue below

        case u.SUSPENDED:                   //only resume/reset irq's active
            if(_is_reset()) return;         //if reset, attach and return
            u.state = last_state;           //else resume, to previous state
            _default();                     //and back to default irq's
            return;
    }

    printf("ISR after switch...\r\n");

    //DEFAULT, ADDRESS, or CONFIGURED state
    //now we can check flags and do important stuff
    if(flags.error){
    }

    if(flags.stall){
    }

    if(flags.sof){
    }

    //and here we go to work moving bytes
    if(flags.token){
        printf("ISR calling token...\r\n");
        UsbConfig::endpoints[stat.endpt].token((uint8_t)stat.bdidx); //call endpoint
    }

}

#endif

/*
________________________________________________________________________________

random notes
________________________________________________________________________________




to perform a remote-wakeup (if my_remote_wakeup = 1)
    Usb::control(RESUMEN, true);
    DelayCP0 tmr;
    tmr.wait_ms(10);
    Usb::control(RESUMEN, false);

 stall
    setting bstall in descriptor also causes epstall (U1EPn) to set
    stalling the endpoint (both tx/rx)
    to clear the stall, clear bstall, then clear epstall (I think)

 control read transfer
    if data sent in (tx) is less than requested AND the size sent is equal
    to the endpoint's max packet size, a 0byte data packet will also need
    to be sent in the next IN packet to signal no more data
    (a less than max packet signals no more data to send, since the host is
    expecting more data if the last packet was a full packet, it then needs
    to be 'told' there is no more data with a 0byte packet)

 enumeration
 usb bus reset by host controller
 rx setup packet- (DEV)GET_DESCRIPTOR from host
 tx packet- descriptor, number of bytes requested
 8th byte in descriptor contains max packet size for endpoint
 WINDOWS will start status stage once it receives 8 bytes
 (rx packet- 0bytes ? I guess), then resets usb bus

 rx setup packet- (DEV)SET_ADDRESS from host
 tx status packet- 0bytes
 set address (now in ADDRESS state)

 rx setup packet- (DEV)GET_DESCRIPTOR from host
 tx packet- descriptor, number of bytes requested
 rx status packet- 0bytes






*/
