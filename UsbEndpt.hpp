#pragma once

#include "Usb.hpp"


class UsbEndpt {
/*______________________________________________________________________________

    endpoint class
    handles all functions for an endpoint

    UsbEndpt ep0(0,UsbEndpt::TRX); //endpoint 0 with rx+tx
    UsbEndpt ep1rx(1,UsbEndpt::TX); //endpoint 1 with tx only
    UsbEndpt ep2tx(2,UsbEndpt::RX); //endpoint 2 with rx only
        endpoint descriptors (4) for endpoint cleared
        endpoint register cleared
        buffer addresses received from UsbBuf (as needed for tx/rx, 2 for each)
        endpoint descriptors init (as needed for tx/rx, count 0)


    ep0.deinit(); //give up buffers, turn off endpoint
    ep0.reinit(); //deinit, then run constructor (init)
        (cannot change n or rx/tx properties)

    ep0.on(true); //turn on endpoint with handshake
    ep0.on(false); //turn off endpoint
    ep1rx.on(ep1rx.RXEN|ep1rx.CTRLDIS); //turn on with specific options


______________________________________________________________________________*/
    public:

    //capabilities
    enum TR { NONE = 0, TX = 1<<2, RX = 1<<3, TRX = 3<<2 };

    //endpoint control register bits
    enum U1EP {
        //LS = 1<<7,            /*HOST mode and U1EP0 only*/
        //RETRYDIS = 1<<6,      /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4,         /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2, ESTALL = 1<<1, HSHAKE = 1<<0
    };

    //public functions
    UsbEndpt(uint8_t, TR); //n, TX|RX|TRX
    void deinit(); //'destructor'-like
    void reinit(); //deinit, then run constructor
    void on(bool); //enable/disable endpoint ([tx],[rx],handshake)
    void on(uint8_t); //enable endpoint with specified U1EP reg bits
    void token(uint8_t); //process transaction complete, called by isr

    //public static function
    static uint32_t bdt_addr(); //to get bdt address (for Usb::bdt_addr() use)



    private:

    //bd entry struct
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

    //endpoint register address, spacing
    enum { U1EP0 = 0xBF808700, U1EP_SPACING = 0x10 };

    //bd entry control bits (out to usb sie)
    enum CTRLOUT {
        UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
        NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<1
    };

    //private functions
    void control(uint8_t);
    volatile uint8_t control();
    volatile uint16_t count();
    void count(uint16_t v);
    volatile uint8_t pid() const;
    void addr(uint8_t* v);
    uint8_t* addr() const;
    volatile bool uown() const;
    volatile bool data01() const;
    volatile bool stall() const;
    void uown(bool);
    void data01(bool);
    void stall(bool);
    void bd_setup(volatile uint8_t*, uint16_t, uint8_t);
    void epreg(U1EP, bool tf);
    volatile bool epreg(U1EP);
    void epreg(uint8_t);
    void bd_init();
    void setup_token();
    void in_token();
    void out_token();
    void rx_giveup();
    void tx_cancel();

    enum SETUPTFER { COMPLETE, IN, OUT, STATUS };

    static bdt_t m_bdt[(my_last_endp+1)*4] __attribute__ ((aligned (512)));
    uint8_t m_endpt_n; //0-15
    TR m_endpt_tr; //TX|RX|TRX (aligned for U1EPn <<2)
    volatile uint8_t* m_endp_reg; //U1EPn sfr register
    volatile uint8_t* m_buf[4]; //pointers to rx-even/odd,tx-even/odd buffers
    uint16_t m_buf_len; //fixed length from UsbBuf (64)
    uint8_t m_tx_idx; //current tx buffer (2/3)
    volatile bdt_t* m_bd[4]; //rx/tx buffer descriptor even/odd
    UsbCh9::SetupPacket_t setup_pkt; //copy of setup data packet (8bytes)
    SETUPTFER setup_stage; //setup transaction stages
    uint8_t m_bd_idx; //current idx into bd (used in functions)
    bool m_data01;
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//static
//UsbHandlers::attach will get bdt address, and set the address in the sfr reg
UsbEndpt::bdt_t UsbEndpt::m_bdt[(my_last_endp+1)*4] = {0};
uint32_t UsbEndpt::bdt_addr(){ return (uint32_t)m_bdt; }



UsbEndpt::UsbEndpt(uint8_t n, TR tr) :
    m_tx_idx(2),
    m_endpt_n(n&15),
    m_endpt_tr(tr),
    m_bd{&m_bdt[n*4],&m_bdt[n*4+1],&m_bdt[n*4+2],&m_bdt[n*4+3]},
    m_endp_reg((volatile uint8_t*)U1EP0+n*U1EP_SPACING),
    m_buf_len(UsbBuf::buf_len()),
    setup_pkt{0},
    setup_stage(COMPLETE),
    m_bd_idx(0),
    m_data01(0)
    {
    //clear endpoint register
    epreg(0);
    //get buffers as needed
    if(tr&RX){ m_buf[0] = UsbBuf::get(); m_buf[1] = UsbBuf::get();}
    if(tr&TX){ m_buf[2] = UsbBuf::get(); m_buf[3] = UsbBuf::get();}
    //all entries set to buffer addresses, 0 count, all control flags cleared
    bd_init();
}
//UsbEndpt::UsbEndpt(uint8_t n, TR tr){}
void UsbEndpt::deinit(){
    //endpoint register- all off
    epreg(0);
    //release all buffers back to UsbBuf::
    for(auto i = 0; i < 2; i++){
        if(m_endpt_tr&RX) UsbBuf::release(m_buf[i]);
        if(m_endpt_tr&TX) UsbBuf::release(m_buf[i+2]);
    }
}
void UsbEndpt::reinit(){ deinit(); UsbEndpt(m_endpt_n,m_endpt_tr); }
void UsbEndpt::control(uint8_t v){ m_bd[m_bd_idx]->control = v; }
volatile uint8_t UsbEndpt::control(){ return m_bd[m_bd_idx]->control; }
volatile uint16_t UsbEndpt::count(){ return m_bd[m_bd_idx]->count; }
void UsbEndpt::count(uint16_t v){ m_bd[m_bd_idx]->count = v; }
volatile uint8_t UsbEndpt::pid() const { return m_bd[m_bd_idx]->pid; }
void UsbEndpt::addr(uint8_t* v){
    m_bd[m_bd_idx]->addr = Reg::k2phys(v); //needs physical address
}
uint8_t* UsbEndpt::addr() const {
    return (uint8_t*)Reg::p2kseg0(m_bd[m_bd_idx]->addr); //return kseg0 address
}
volatile bool UsbEndpt::uown() const { return m_bd[m_bd_idx]->uown; }
volatile bool UsbEndpt::data01() const { return m_bd[m_bd_idx]->data01; }
volatile bool UsbEndpt::stall() const { return m_bd[m_bd_idx]->bstall; }
void UsbEndpt::uown(bool tf){ m_bd[m_bd_idx]->uown = tf; }
void UsbEndpt::data01(bool tf){ m_bd[m_bd_idx]->data01 = tf; }
void UsbEndpt::stall(bool tf){ m_bd[m_bd_idx]->bstall = tf; }
void UsbEndpt::bd_setup(
    volatile uint8_t* a, uint16_t c, uint8_t f){
    addr((uint8_t*)a); count(c); control(f);
}
void UsbEndpt::epreg(U1EP e, bool tf){ Reg::set(m_endp_reg, e, tf); }
volatile bool UsbEndpt::epreg(U1EP e){ return Reg::is_set8(m_endp_reg, e); }
void UsbEndpt::epreg(uint8_t v){ Reg::val8(m_endp_reg, v); }
void UsbEndpt::bd_init(){
    for(auto i = 0; i < 2; i++){
        if(m_endpt_tr&RX){
            m_bd_idx = i;
            bd_setup(m_buf[i], 0, 0);
        }
        if(m_endpt_tr&TX){
            m_bd_idx = i;
            bd_setup(m_buf[i+2], 0, 0);
        }
    }
}
void UsbEndpt::on(bool tf){ if(tf) epreg(m_endpt_tr|HSHAKE); else epreg(0); }
void UsbEndpt::on(uint8_t v){ epreg(v); }
void UsbEndpt::rx_giveup(){ count(m_buf_len); uown(true); }
void UsbEndpt::tx_cancel(){
    //clear any pending tx
    uint8_t idx = m_bd_idx; //save first
    m_bd_idx = m_tx_idx; control(0); //current tx
    m_bd_idx = m_tx_idx^1; control(0); //other tx
    m_bd_idx = idx; //restore
}


/*..............................................................................
 process transaction complete
    called from ISR with index into this endpoint
    (0-3, rx-even,rx-odd,tx-even,tx-odd)
..............................................................................*/
void UsbEndpt::token(uint8_t idx){
    //check if we even have an endpoint in use
    //if this is an unused endpoint below a higher used endpoint
    //turn off (should already be off, and shouldn't be here)
    if(m_endpt_tr = NONE){ on(false); return; }

    //which bd entry was used for this TRNIF
    m_bd_idx = idx;

    //pid from bd entry (should only see 1,9,13)
    switch(pid()){

        case UsbCh9::SETUP:
            //save setup data packet for later
            setup_pkt = *(UsbCh9::SetupPacket_t*)&m_buf[m_bd_idx];
            rx_giveup();
            tx_cancel();
            m_data01 = 0;
            setup_token();
            Usb::control(Usb::PKTDIS, false); //continue control transfer
            break;

        case UsbCh9::IN:
            in_token();
            break;

        case UsbCh9::OUT:
            out_token();
            rx_giveup();
            break;
    }
}

/*..............................................................................

..............................................................................*/
void UsbEndpt::setup_token(){
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
            //setup_pkt.wLength should = 2
            //2bytes- byte0/bit0 = self-powered?, byte0/bit1=remote wakeup?
            m_buf[m_tx_idx][0] = my_self_powered<<0;
            m_buf[m_tx_idx][0] |= my_remote_wakeup<<1;
            m_buf[m_tx_idx][1] = 0;
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

    if(setup_pkt.wLength == 0){
        setup_stage = STATUS;
        m_data01 = 1;
    }
    else setup_stage = setup_pkt.bmRequestType & 0x80 ? IN : OUT;

    //changing m_bd_idx - should be ok, will exit shortly
    m_bd_idx = m_tx_idx;
    bd_setup(m_buf[m_tx_idx], setup_pkt.wLength, UOWN|m_data01);
    m_tx_idx ^= 1;
}
/*..............................................................................

..............................................................................*/
void UsbEndpt::in_token(){
    if(setup_stage == STATUS){
        setup_stage = COMPLETE;

    }
    if(setup_stage == IN){}
    if(setup_stage == OUT){}

}
/*..............................................................................

..............................................................................*/
void UsbEndpt::out_token(){
    if(setup_stage == STATUS){
        setup_stage = COMPLETE;

    }
    if(setup_stage == IN){}
    if(setup_stage == OUT){}

}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/*..............................................................................

..............................................................................*/