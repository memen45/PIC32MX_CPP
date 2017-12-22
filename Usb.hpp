#pragma once

/*=============================================================================
 USB peripheral - pic32mmxxxxGPMxxx
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "UsbCh9.hpp"
#include "Pins.hpp"
#include "Irq.hpp"




///////////////////////////////////////////////////////////////////////////////
/////// user provided data ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const bool my_self_powered = 1;      //self-powered=1, bus-powered=0
static bool my_remote_wakeup = 1;           //remote wakeup enabled=1
static const uint8_t my_last_endp = 1;      //last endpoint number used
static const uint8_t my_n_endp = 2;         //number of endpoints used
static const uint8_t my_buffer_size = 64;   //size of buffers (all same)
static const uint8_t my_buffer_count = 8;   //number of buffers in buffer pool

static Pins vbus_pin(Pins::B6);             //Vbus pin
static const uint8_t usb_irq_pri = 5;       //usb interrupt priority
static const uint8_t usb_irq_subpri = 0;    //usb interrupt sub-priority
//IPLn will need to match usb_irq_pri,
//vector(n) will need to match USB vector number
//if using shadow register for this priority, will have to set elsewhere
//using Irq::shadow_set(pri, 1), then match the priority here and use IPLnSRS
//declare here in C namespace so we can have in user data here,
//then define in code below
//(its in UsbHandler section, although not in any class)
extern "C" {
    void __attribute__((vector(29), interrupt(IPL5SOFT))) UsbISR();
}
///////////////////////////////////////////////////////////////////////////////
/////// usb uses irq only, no polling /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



//__inline include______________________________________________________________

#include "UsbBuf.hpp"
//______________________________________________________________________________



//__inline include______________________________________________________________

#include "UsbBdt.hpp"
//______________________________________________________________________________




struct Usb {
//______________________________________________________________________________

//______________________________________________________________________________

    /*..........................................................................
     flags, interrupts
        FLAGS enum contains all flag/irq names

        flags();                -return val of all flags (uint8_t)
        flag(FLAGS);            -get specified flag
        flags_clr(uint8_t);     -clear flag(s) as bitmask (1=clear)

        irqs();                 -return val of all irqs (uin8_t)
        irq(FLAGS);             -get specified irq
        irqs(uint8t);           -set value (all)
        irq(FLAGS, bool);       -one irq on/off (true=on)

        eflag, eirq functions mirror above but are for error flags
    ..........................................................................*/

    enum FLAGS : uint8_t {
        //U1IR, U1IE
        STALL = 1<<7,
        ATTACH = 1<<6, //host only
        RESUME = 1<<5,
        IDLE = 1<<4,
        TOKEN = 1<<3,
        SOF = 1<<2,
        ERROR = 1<<1,
        RESET = 1<<0,
        DETACH = 1<<0, //host only
        ALLFLAGS = 255
    };

    enum EFLAGS : uint8_t {
        //U1EIR, U1EIE
        BITSTUFF = 1<<7,
        BUSMATRIX = 1<<6,
        DMA = 1<<5,
        BUSTIMEOUT = 1<<4,
        DATASIZE = 1<<3,
        CRC16 = 1<<2,
        CRC5 = 1<<1,
        EOF = 1<<1,
        PID = 1<<0,
        ALLEFLAGS = 255
    };

    static volatile uint8_t flags       ();             //get all
    static volatile bool    flag        (FLAGS);        //get one
    static void             flags_clr   (uint8_t);      //clear one or more

    static uint8_t          irqs        ();             //get all
    static bool             irq         (FLAGS);        //get one
    static void             irqs        (uint8_t);      //set value (all)
    static void             irq         (FLAGS, bool);  //irq on/off

    static volatile uint8_t eflags      ();             //get all
    static volatile bool    eflag       (EFLAGS);       //get one
    static void             eflags_clr  (uint8_t);      //clear one or more

    static uint8_t          eirqs       ();             //get all
    static bool             eirq        (EFLAGS);       //get one
    static void             eirqs       (uint8_t);      //set value (all)
    static void             eirq        (EFLAGS, bool); //eirq on/off


    /*..........................................................................
     power
        POWER enum contains all power names

        power(POWER);           -return POWER bit is set
        power(POWER, bool);     -set/clr specified POWER bit (true=on)
    ..........................................................................*/

    enum POWER : uint8_t {
        //U1PWRC
        PENDING = 1<<7,
        SLEEPGUARD = 1<<4,
        BUSY = 1<<3,
        SUSPEND = 1<<1,
        USBPWR = 1<<0
    };

    static bool     power               (POWER);
    static void     power               (POWER, bool);

    /*..........................................................................
     stat
        only valid when TOKEN flag set
    ..........................................................................*/
    typedef union {
        struct {
        unsigned :2; unsigned eveodd:1; unsigned trx:1; unsigned endpt:4;
        };
        struct { unsigned :2; unsigned bdn:6; };
        uint8_t val;
    } stat_t;
    static volatile uint8_t stat                ();
    static void             stat                (stat_t&);


    /*..........................................................................
     control

    ..........................................................................*/
    enum CONTROL : uint8_t {
        JSTATE = 1<<7,
        SE0 = 1<<6,
        PKTDIS = 1<<5,
        TOKBUSY = 1<<5,
        USBRESET = 1<<4,
        HOSTEN = 1<<3,
        RESUMEN = 1<<2,
        PPRESET = 1<<1,
        USBEN = 1<<0,
        SOFEN = 1<<0
    };

    static volatile bool    control             (CONTROL);
    static void             control             (CONTROL, bool);
    static void             control             (uint8_t);


    /*..........................................................................
     dev_addr
        usb device address get/set
    ..........................................................................*/
    static uint8_t  dev_addr            ();
    static void     dev_addr            (uint8_t);


    /*..........................................................................
     frame
        get frame number
    ..........................................................................*/
    static uint16_t frame               ();


    /*..........................................................................
     bdt_addr
        get/set bdt table address
    ..........................................................................*/
    static void         bdt_addr            (uint32_t);
    static uint32_t     bdt_addr            ();


    /*..........................................................................
     config
        get/set config register
    ..........................................................................*/
    enum CONFIG : uint8_t {
        EYETEST = 1<<7,
        OEMON = 1<<6,
        SIDLE = 1<<4,
        LSDEV = 1<<3,
        AUTOSUSP = 1<<0
    };

    static void     config              (CONFIG, bool); //set 1bit
    static bool     config              (CONFIG);       //get 1bit
    static void     config              (uint8_t);      //set reg val


    /*..........................................................................
     endp
        get/set endpoint n
        clear all endpoint registers
    ..........................................................................*/
    enum EPV {
        LS = 1<<7,          /*HOST mode and U1EP0 only*/
        RETRYDIS = 1<<6,    /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4,     /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2,
        ESTALL = 1<<1, HSHAKE = 1<<0
    };

    static void     endp                (uint8_t, EPV, bool);
    static bool     endp                (uint8_t, EPV);
    static void     endp                (uint8_t, uint8_t);
    static void     endps_clr           ();

    /*..........................................................................
     state
        usb bus state
    ..........................................................................*/
    typedef enum {
        DETACHED,   //usb peripheral shut down (I made this state up)
        ATTACHED,   //usb is init, but not powered
        POWERED,    //vbus now has power, but no reset seen yet
        DEFAULT,    //reset received, device address is 0 (default)
        ADDRESS,    //now have unique device address
        CONFIGURED, //now configured (and not in suspended state)
        SUSPENDED   //no activity for >3ms
    } state_t;

    static state_t state;



    private:

    //registers - all registers use only first 8bits
    enum {
        U1PWRC = 0xBF808480,
        U1IR = 0xBF808600,//no SET, INV
        U1IE = 0xBF808610,
        U1EIR = 0xBF808620,//no SET, INV
        U1EIE = 0xBF808630,
        U1STAT = 0xBF808640,//no SET, INV, CLR
        U1CON = 0xBF808650,
        U1ADDR = 0xBF808660,
        U1BDTP1 = 0xBF808670,
        U1FRML = 0xBF808680, //no SET, INV, CLR
        U1FRMH = 0xBF808690, //no SET, INV, CLR
        U1BDTP2 = 0xBF8086C0,
        U1BDTP3 = 0xBF8086D0,
        U1CNFG1 = 0xBF8086E0,
        U1EP0 = 0xBF808700, U1EP_SPACING = 0x10
     };

};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Usb static functions, vars
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Usb::state_t Usb::state = Usb::DETACHED;
bool Usb::power(POWER e){ return Reg::is_set8(U1PWRC, e); }
void Usb::power(POWER e, bool tf){ Reg::set(U1PWRC, e, tf); }
//get all
volatile uint8_t Usb::flags(){ return Reg::val8(U1IR); }
volatile uint8_t Usb::eflags(){ return Reg::val8(U1EIR); }
//get one
volatile bool Usb::flag(FLAGS e){ return Reg::is_set8(U1IR, e); }
volatile bool Usb::eflag(EFLAGS e){ return Reg::is_set8(U1EIR, e); }
//clear one or more
void Usb::flags_clr(uint8_t v){ Reg::val8(U1IR, v); }
void Usb::eflags_clr(uint8_t v){ Reg::val8(U1EIR, v); }
//get all
uint8_t Usb::irqs(){ return Reg::val8(U1IE); }
uint8_t Usb::eirqs(){ return Reg::val8(U1EIE); }
//get one
bool Usb::irq(FLAGS e){ return Reg::is_set8(U1IE, e); }
bool Usb::eirq(EFLAGS e){ return Reg::is_set8(U1EIE, e); }
//set value
void Usb::irqs(uint8_t v){ Reg::val8(U1IE, v); }
void Usb::eirqs(uint8_t v){ Reg::val8(U1EIE, v); }
//one on/off
void Usb::irq(FLAGS e, bool tf){ Reg::set(U1IE, e); }
void Usb::eirq(EFLAGS e, bool tf){ Reg::set(U1EIE, e); }

volatile uint8_t Usb::stat(){ return Reg::val8(U1STAT); }
void Usb::stat(stat_t& s){ s.val = Reg::val8(U1STAT); }

volatile bool Usb::control(CONTROL e){ return Reg::is_set8(U1CON, e); }
void Usb::control(CONTROL e, bool tf){ Reg::set(U1CON, e, tf); }
void Usb::control(uint8_t v){ Reg::val8(U1CON, v); }

uint8_t Usb::dev_addr(){ return Reg::val8(U1ADDR) & 127; }
void Usb::dev_addr(uint8_t v){ Reg::val8(U1ADDR, v & 127); }

uint16_t Usb::frame(){ return (Reg::val8(U1FRMH)<<8) | Reg::val8(U1FRML); }

void Usb::bdt_addr(uint32_t v){
    v = Reg::k2phys(v); //physical address
    Reg::val8(U1BDTP1, v>>8); //512byte aligned (bit0 of this reg unused)
    Reg::val8(U1BDTP2, v>>16);
    Reg::val8(U1BDTP3, v>>24);
}
uint32_t Usb::bdt_addr(){
    return  Reg::p2kseg0( (uint32_t)
    Reg::val8(U1BDTP1)<<8 | Reg::val8(U1BDTP2)<<16 | Reg::val8(U1BDTP3)<<24
    ); //kseg0
}

void Usb::config(CONFIG e, bool tf){ Reg::set(U1CNFG1, e, tf); }
bool Usb::config(CONFIG e){ return Reg::is_set8(U1CNFG1, e); }
void Usb::config(uint8_t v){ Reg::val8(U1CNFG1, v); }

void Usb::endp(uint8_t n, EPV e, bool tf){
    Reg::set(U1EP0+(n&15)*U1EP_SPACING, e, tf);
}
bool Usb::endp(uint8_t n, EPV e){
    return Reg::is_set8(U1EP0+(n&15)*U1EP_SPACING, e);
}
void Usb::endp(uint8_t n, uint8_t v){
    Reg::val8(U1EP0+(n&15)*U1EP_SPACING, v);
}
void Usb::endps_clr(){
    for(auto i = 0; i < 16; i++){
        Reg::val8(U1EP0+i*U1EP_SPACING, 0);
    }
}
//______________________________________________________________________________





struct UsbHandlers {
//______________________________________________________________________________
//
// UsbHandlers - handle usb interrupt, call handlers
//______________________________________________________________________________

    static void     UsbISR              (void);
    static void     init                ();
    //token complete dispatch
    static void     token               ();
    //token handlers
    static void     setup               (Usb::stat_t&);
    static void     in                  (Usb::stat_t&);
    static void     out                 (Usb::stat_t&);
    //state handlers
    static void     detach              (void);
    static void     attach              (void);


    //rx even/odd buffers
    static volatile uint8_t* endp0_rx[2];
    //tx even/odd buffers (need not be volatile, but is easier if same as rx)
    static volatile uint8_t* endp0_tx[2];

    //flags for endpoint 0 transmit buffers
    static bool endp0_tx_eveodd;
    static bool endp0_tx_data01;

    enum { COMPLETE, IN, OUT, STATUS };
    static uint8_t  setup_stage;

    static UsbCh9::SetupPacket_t setup_pkt;

};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// UsbHandlers static functions, vars
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
volatile uint8_t* UsbHandlers::endp0_rx[2] = { UsbBuf::get(), UsbBuf::get() };
volatile uint8_t* UsbHandlers::endp0_tx[2] = { UsbBuf::get(), UsbBuf::get() };
bool UsbHandlers::endp0_tx_eveodd;
bool UsbHandlers::endp0_tx_data01;
uint8_t  UsbHandlers::setup_stage = UsbHandlers::COMPLETE;
UsbCh9::SetupPacket_t UsbHandlers::setup_pkt = {0};


/*..............................................................................
    USB ISR
    declared in user data at top of this file
..............................................................................*/
void  UsbISR(){
    Usb u;
    //keep track of usb state for resumes
    static Usb::state_t last_state;
    //get all flags
    uint8_t flags = u.flags();
    //clear irq flag now (before any early returns)
    Irq::flagclear(Irq::USB);

    //ATTACHED->POWERED if vbus_pin high
    if(u.state == u.ATTACHED){
        if(vbus_pin.ison()) u.state = u.POWERED;
        else { //no power (not sure how we would get irq with no vbus)
            u.eflags_clr(u.ALLEFLAGS);
            u.flags_clr(u.ERROR);
            return;
        }
    }

    //must be >= POWERED

    //check if resume from SUSPENDED
    if (flags & u.RESUME){
        u.state = last_state; //back to previous state
        u.flags_clr(u.RESUME);
    }

    //check if need to suspend (idle detected >3ms)
    if (flags & u.IDLE){
        last_state = u.state; //save
        u.state = u.SUSPENDED;
        //do suspend- whatever is needed
    }

    //check reset
    //POWERED->DEFAULT, or >=DEFAULT->ATTACHED
    if(flags & u.RESET){ //handle reset condition
        if(u.state == u.POWERED){
            u.state = u.DEFAULT;
            u.flags_clr(u.RESET);
        }
        else UsbHandlers::attach(); //from >=DEFAULT, so attach
        return;
    }

    //check if suspended- then nothing more to do
    if(u.state == u.SUSPENDED){
        //not sure which flags would get us here
        //clear them all
        u.eflags_clr(u.ALLEFLAGS);
        u.flags_clr(u.ALLFLAGS);
        return;
    }


    //in state DEFAULT, ADDRESS, or CONFIGURED

    if(flags & u.ERROR){ //handle errors
        u.eflags_clr(u.ALLEFLAGS);
        u.flags_clr(u.ERROR);
        return;
    }

    if(flags & u.STALL){ //handle stall
        u.flags_clr(u.STALL);
    }

    if(flags & u.SOF){ //handle SOF
        u.flags_clr(u.SOF);
    }

    if(flags & u.TOKEN){
        do{ //get while TOKEN flag set (stat buffer is 4 deep)
            UsbHandlers::token();
            u.flags_clr(u.TOKEN); //do after, stat fifo advances when cleared
        } while(u.flag(u.TOKEN));
    }

}

/*..............................................................................
    handle token processing complete flag TRNIF
    dispatch to setup(), in(), out()
..............................................................................*/
void UsbHandlers::token(){
    UsbBdt ubdt;
    Usb u;

    //get stat (endpoint, tx/rx, dir)
    static Usb::stat_t s;
    u.stat(s);

    switch(ubdt.pid(s.bdn)){

    case UsbCh9::SETUP: //ep0 only
        //|setup-token|data0|ack|
        //      ^        ^---rxbuffer
        //      ^---pid(=SETUP)
        //copy the setup data (rxbuffer) to our static struct
        //setup packet always 8bytes (not checked)
        setup_pkt = *(UsbCh9::SetupPacket_t*)endp0_rx[s.eveodd];

        //done with the rx buffer, reset count, give back to usb
        ubdt.count(s.bdn, my_buffer_size);
        ubdt.control(s.bdn, ubdt.UOWN);

        //clear any ep0-tx
        ubdt.control(2, 0); //ep0-tx-even
        ubdt.control(3, 0); //ep0-tx-odd

        //process
        setup(s);

        //clear PKTDIS (set when SETUP transaction received)
        u.control(u.PKTDIS, false);

        break;

    case UsbCh9::IN:
        in(s);
        break;

    case UsbCh9::OUT:
        out(s);
        break;
    }
}

/*..............................................................................
    process setup token

..............................................................................*/
void UsbHandlers::setup(Usb::stat_t& s){
    Usb u; UsbBdt ubdt;

    switch(setup_pkt.wRequest){

    case UsbCh9::DEV_CLEAR_FEATURE:
        //setup_pkt.wValue
        break;
    case UsbCh9::DEV_SET_FEATURE:
        //setup_pkt.wValue
        break;
    //case UsbCh9::DEV_SET_ADDRESS:
        //do after status
        //break;
    case UsbCh9::DEV_SET_CONFIGURATION:
        //setup_pkt.wValue
        break;
    case UsbCh9::DEV_GET_STATUS:
        //2bytes- byte0/bit0 = self-powered?, byte0/bit1=remote wakeup?
        endp0_tx[endp0_tx_eveodd][0] = my_self_powered<<0;
        endp0_tx[endp0_tx_eveodd][0] |= my_remote_wakeup<<1;
        endp0_tx[endp0_tx_eveodd][1] = 0;
        break;
    case UsbCh9::DEV_GET_DESCRIPTOR:
        //
        break;
    case UsbCh9::DEV_SET_DESCRIPTOR:
        //
        break;
    case UsbCh9::DEV_GET_CONFIGURATION:
        //return 1 byte- 0=not configured
        //
        break;
    }

    if(setup_pkt.wLength == 0) setup_stage = STATUS;
    else setup_stage = setup_pkt.bmRequestType & 0x80 ? IN : OUT;

    ubdt.esetup(0|1|0, (uint8_t*)endp0_tx[endp0_tx_eveodd], setup_pkt.wLength,
        ubdt.UOWN|setup_stage == STATUS ? ubdt.DATA01 : 0);

}

/*..............................................................................


..............................................................................*/
void UsbHandlers::in(Usb::stat_t& s){
    UsbBdt ubdt; Usb u;

    //setup status stage IN complete
    if(setup_stage == STATUS){
        setup_stage = COMPLETE;
        //give back rx buffer to usb
        ubdt.count(s.bdn, my_buffer_size);
        ubdt.control(s.bdn, ubdt.UOWN);

        //if was set_address, do it now
        if(setup_pkt.wRequest == UsbCh9::DEV_SET_ADDRESS){
            u.dev_addr(setup_pkt.wValue); //device address
            u.state = u.ADDRESS; //now have address, change usb state
        }
        return;
    }


    //handle other in's


}

/*..............................................................................


..............................................................................*/
void UsbHandlers::out(Usb::stat_t& s){

    //status stage received on data1
    //(not checked if was data1, or length was 0, will assume because
    // I don't know what to do yet if it is not)
    if(setup_stage == STATUS){
        setup_stage = COMPLETE;
        //nothing more to do
        return;
    }

    //handle other out's


}

/*..............................................................................


..............................................................................*/
void UsbHandlers::detach(void){
    Usb u;
    u.control(u.USBEN, false);  //disable usb module, detach from bus
    Irq::on(Irq::USB, false);   //all usb irqs off
    while(u.power(u.BUSY));     //wait for busy bit to clear first
    u.power(u.USBPWR, true);    //usb module off

    u.state = u.DETACHED;
    //wait 100ms+ before attach again
    //(if was previously in >= DEFAULT state)
}

/*..............................................................................


..............................................................................*/
void UsbHandlers::attach(void){
    Usb u; UsbBdt ubdt; UsbBuf ubuf;

    u.power(u.USBPWR, true);    //usb on (all regs should be reset now
                                //if power was previously off)

    u.bdt_addr(ubdt.init_all());//clear all bdt entries
                                //and set bdt table address


    ubuf.reinit();              //clear all buffers, clear inuse flag
    endp0_rx[0] = (uint8_t*)ubuf.get(); //get buffers for
    endp0_rx[1] = (uint8_t*)ubuf.get(); //endp0 rx even/odd
    endp0_tx[0] = (uint8_t*)ubuf.get(); //get buffers for
    endp0_tx[1] = (uint8_t*)ubuf.get(); //endp0 tx even/odd
    endp0_tx_eveodd = 0; //even
    endp0_tx_data01 = 0; //data0
                                //these never released back to UsbBuf

    u.dev_addr(0);              //set device address to 0 (should already be)

    //endpoint0 init
    u.endp(0, u.RXEN|u.TXEN|u.HSHAKE);
    //both ep0 rx address set to buffers
    //should be able to leave these alone from now on
    ubdt.addr(0|0|0, (uint8_t*)endp0_rx[0]);
    ubdt.addr(0|0|1, (uint8_t*)endp0_rx[1]);
    //and set count
    ubdt.count(0|0|0, ubuf.buf_len());
    ubdt.count(0|0|1, ubuf.buf_len());
    //give up ep0-rx-even/odd to usb
    ubdt.control(0|0|0, ubdt.UOWN);
    ubdt.control(0|0|1, ubdt.UOWN);
    //usb now has a buffer to rx first setup data

    //reset setup token
    setup_stage = COMPLETE;

    //enable irqs
    u.irqs(u.STALL|u.IDLE|u.TOKEN|u.SOF|u.ERROR|u.RESET);
    u.eirqs(u.BITSTUFF|u.BUSTIMEOUT|u.DATASIZE|u.CRC16|u.CRC5|u.PID);
    Irq::init(Irq::USB, usb_irq_pri, usb_irq_subpri, true);

    //enable usb
    u.control(u.USBEN, true);

    //if vbus is high- in powered state
    u.state = vbus_pin.ison() ? u.POWERED : u.ATTACHED;
}

/*..............................................................................


..............................................................................*/
 void UsbHandlers::init(){
    //USBEN=0, USBPWR=0, Irq::USB off, state = DETACHED
    detach();

    //vbus/rb6 to input (should already be input on reset)
    //but do anyway
    vbus_pin.digital_in();
    //pulldown when no vbus keeps pin low
    vbus_pin.pulldn(true);
    //can also setup irq for vbus_pin state change
    //which then calls detach/attach

    //init all things
    attach();
 }



/*______________________________________________________________________________

    endpoint class
    handles all functions for an endpoint

    UsbEndpt ep0(0,3); //endpoint 0 with rx+tx
    UsbEndpt ep1rx(1,1); //endpoint 1 with rx only
    UsbEndpt ep2tx(2,2); //endpoint 1 with rx only
        endpoint descriptors (4) for endpoint cleared
        endpoint register cleared
        buffer addresses received from UsbBuf (as needed for tx/rx, 2 for each)
        endpoint descriptors init (as needed for tx/rx, count 0)


    ep0.deinit(); //give up buffers, turn off endpoint
    ep0.reinit(); deinit, then run constructor
        (cannot change n or rx/tx properties)

    ep0.on(true); //turn on endpoint with handshake
    ep0.on(false); //turn off endpoint
    ep1rx.on(ep1rx.RXEN|ep1rx.CTRLDIS); //turn on with specific options


______________________________________________________________________________*/
class UsbEndpt {

    private:

    typedef union {
        struct { uint32_t dat; uint32_t addr; };
        struct { unsigned :16; uint16_t count; };
        struct {
            unsigned :2;
            unsigned bstall:1;
            unsigned dts:1;
            unsigned ninc:1;
            unsigned keep:1;
            unsigned data01:1;
            unsigned uown:1;
        };
        struct { unsigned :2; unsigned pid:4; };
        uint8_t control;
        uint64_t all;
    } bdt_t;

    enum { U1EP0 = 0xBF808700, U1EP_SPACING = 0x10 };

    public:

    enum U1EP {
        LS = 1<<7,          /*HOST mode and U1EP0 only*/
        RETRYDIS = 1<<6,    /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4,     /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2,
        ESTALL = 1<<1, HSHAKE = 1<<0
    };

    enum CTRL {
        UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
        NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<1
    };

    UsbEndpt(uint8_t, uint8_t); //n, rx/tx/rx+tx (2,1,3)
    void deinit(); //manual 'destructor'
    void reinit(); //deinit, then run constructor
    void on(bool); //enable/disable endpoint ([tx],[rx],handshake)
    void on(uint8_t); //enable endpoint with U1EP reg bits

    private:

    void control(volatile bdt_t*, uint8_t);
    volatile uint8_t control(volatile bdt_t*);
    volatile uint16_t count(volatile bdt_t*);
    void count(volatile bdt_t*, uint16_t v);
    volatile uint8_t pid(volatile bdt_t* bp) const;
    void addr(volatile bdt_t* bp, uint8_t* v);
    uint8_t* addr(volatile bdt_t* bp) const;
    volatile bool uown(volatile bdt_t*) const;
    volatile bool data01(volatile bdt_t*) const;
    volatile bool stall(volatile bdt_t*) const;
    void uown(volatile bdt_t*, bool);
    void data01(volatile bdt_t*, bool);
    void stall(volatile bdt_t*, bool);
    void bd_setup(volatile bdt_t*, volatile uint8_t*, uint16_t, uint8_t);
    void epreg(U1EP, bool tf);
    volatile bool epreg(U1EP);
    void epreg(uint8_t);
    void bd_init();


    const uint8_t m_endpt_n; //0-15
    const uint8_t m_endpt_tr; //2=rx,1=tx,3=tx+rx
    volatile uint8_t* m_endp_reg; //U1EPn sfr register
    volatile uint8_t* m_buf_rx[2]; //pointers to rx even/odd buffers
    volatile uint8_t* m_buf_tx[2]; //pointers to txx even/odd buffers
    const uint16_t m_buf_len; //fixed length from UsbBuf
    bool m_tx_idx; //current tx buffer (0/1)
    volatile bdt_t* m_bd_rx[2]; //rx buffer descriptor even/odd
    volatile bdt_t* m_bd_tx[2]; //tx buffer descriptor even/odd
};

UsbEndpt::UsbEndpt(uint8_t n, uint8_t tr) :
    m_tx_idx(0),
    m_endpt_n(n&15),
    m_endpt_tr(tr&3),
    m_bd_rx{(volatile bdt_t*)UsbBdt::init(n),
            (volatile bdt_t*)UsbBdt::init(n+1)},
    m_bd_tx{(volatile bdt_t*)UsbBdt::init(n+2),
            (volatile bdt_t*)UsbBdt::init(n+3)},
    m_endp_reg((volatile uint8_t*)U1EP0+n*U1EP_SPACING),
    m_buf_len(UsbBuf::buf_len())
    {
    //clear endpoint register
    epreg(0);
    //get buffers as needed
    if(tr&2){ m_buf_rx[0] = UsbBuf::get(); m_buf_rx[1] = UsbBuf::get();}
    if(tr&1){ m_buf_tx[0] = UsbBuf::get(); m_buf_tx[1] = UsbBuf::get();}
    //all entries set to buffer addresses, 0 count, all control flags cleared
    bd_init();
}
void UsbEndpt::deinit(){
    //endpoint register- all off
    epreg(0);
    //release all buffers back to UsbBuf::
    for(auto i = 0; i < 2; i++){
        if(m_endpt_tr&2) UsbBuf::release(m_buf_rx[i]);
        if(m_endpt_tr&1) UsbBuf::release(m_buf_tx[i]);
    }
}
void UsbEndpt::reinit(){ deinit(); UsbEndpt(m_endpt_n,m_endpt_tr); }
void UsbEndpt::control(volatile bdt_t* bp, uint8_t v){ bp->control = v; }
volatile uint8_t UsbEndpt::control(volatile bdt_t* bp){ return bp->control; }
volatile uint16_t UsbEndpt::count(volatile bdt_t* bp){ return bp->count; }
void UsbEndpt::count(volatile bdt_t* bp, uint16_t v){ bp->count -= v; }
volatile uint8_t UsbEndpt::pid(volatile bdt_t* bp) const { return bp->pid; }
void UsbEndpt::addr(volatile bdt_t* bp, uint8_t* v){
    bp->addr = Reg::k2phys(v);
}
uint8_t* UsbEndpt::addr(volatile bdt_t* bp) const {
    return (uint8_t*)Reg::p2kseg0(bp->addr);
}
volatile bool UsbEndpt::uown(volatile bdt_t* bp) const { return bp->uown; }
volatile bool UsbEndpt::data01(volatile bdt_t* bp) const { return bp->data01; }
volatile bool UsbEndpt::stall(volatile bdt_t* bp) const { return bp->bstall; }
void UsbEndpt::uown(volatile bdt_t* bp, bool tf){ bp->uown = tf; }
void UsbEndpt::data01(volatile bdt_t* bp, bool tf){ bp->data01 = tf; }
void UsbEndpt::stall(volatile bdt_t* bp, bool tf){ bp->bstall = tf; }
void UsbEndpt::bd_setup(
    volatile bdt_t* bp, volatile uint8_t* a, uint16_t c, uint8_t f){
    addr(bp, (uint8_t*)a); count(bp, c); control(bp, f);
}
void UsbEndpt::epreg(U1EP e, bool tf){ Reg::set(m_endp_reg, e, tf); }
volatile bool UsbEndpt::epreg(U1EP e){ return Reg::is_set8(m_endp_reg, e); }
void UsbEndpt::epreg(uint8_t v){ Reg::val8(m_endp_reg, v); }
void UsbEndpt::bd_init(){
    for(auto i = 0; i < 2; i++){
        if(m_endpt_tr&2) bd_setup(m_bd_rx[i], m_buf_rx[i], 0, 0);
        if(m_endpt_tr&1) bd_setup(m_bd_tx[i], m_buf_tx[i], 0, 0);
    }
}
void UsbEndpt::on(bool tf){ if(tf) epreg(m_endpt_tr<<2|HSHAKE); else epreg(0); }
void UsbEndpt::on(uint8_t v){ epreg(v); }




 /*
  ______________________________________________________________________________

  random notes
  ______________________________________________________________________________




  to perform a remote-wakeup (if my_remote_wakeup = 1)
    Usb::control(RESUMEN, true);
    DelayCP0 tmr;
    tmr.wait_ms(10);
    Usb::control(RESUMEN, false);




  */