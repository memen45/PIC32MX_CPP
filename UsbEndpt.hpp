#pragma once

#include "Usb.hpp"
#include "Reg.hpp"



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




class UsbEndptRx {
/*______________________________________________________________________________

 RX endpoint class
    handles only RX endpoints in specific endpoint
    UsbEndpt uses this class for RX operations
    UsbEndpt in charge of U1EPn

    constructor needs the bdt base address, endpoint n, and max packet size
    (if max packet size set to 0, this RX is not used- start() will always
     return false)

    reinit()
        -clears all private data, bd entries (also used in constructor)
        -no return value
    start(*buf,count)
        -starts a rx into buf, for count bytes
        -returns true if started, false if cannot (any buffer busy)
    check()
        -after an OUT/SETUP irq, checks if any more data to receive from start()
        -if not complete, returns bytes left to rx
        -if completed, returns 0

    to ready endpoint 0 for a setup (in UsbEndpt::)-
        m_ep_rx.start(my_buf, 8);
        //in token check code for setup
        if(m_ep_rx.check() == 0){ we have a setup packet in my_buf }
        //or maybe just
        m_ep_check(); //since we have a setup transaction completed- must be ok


______________________________________________________________________________*/
    public:
                UsbEndptRx  (bdt_t*, uint8_t, uint16_t);
    void        reinit      ();
    bool        start       (uint8_t*, uint16_t);
    uint16_t    check       ();

    private:
    void        rx_setup    (uint16_t n);
    void        rx_stop     ();

    private:
    bdt_t*              m_bd[2];        //even/odd bd entry pointers
    const uint16_t      m_max_count;    //max data packet size
    uint32_t            m_bufptr;       //current buffer pointer (phys address)
    uint8_t             m_bufn;         //how many buffers in use (0,1,2)
    uint8_t             m_eveodd;       //even/odd entry to use next
    int16_t             m_count;        //keep track of rx count
                                        //int16_t so can do 0-1023
                                        //and also see if <0 (too much rx)
};

/*______________________________________________________________________________

 init (constructor) or reinit rx endpoint
 set bd rx addresses (even/odd) of endpoint n (constructor only)
 (passed in via bdt_t* which points to bdt, we want first two from computed
  index into bdt, which are both rx)
 also need to know max packet size (constructor only)
 all vars reset, bd entries cleared
______________________________________________________________________________*/
UsbEndptRx::UsbEndptRx(bdt_t* b, uint8_t n, uint16_t max)
    : m_bd{&b[n*4], &b[n*4+1]}, m_max_count(max)
{
    reinit();
}
void UsbEndptRx::reinit(){
    m_bufptr = 0;
    m_bufn = 0;
    m_eveodd = 0;
    m_count = 0;
    m_bd[0]->all = 0;
    m_bd[1]->all = 0;
}

/*______________________________________________________________________________

 check if still receiving data- return bytes remaining, or 0 if done
 called only from UsbEndpt:: when TRNIF and token type is OUT/SETUP
 use U1STAT to check which buffer (even/odd) caused the irq
 (must be a buffer returned back to us from usb, so dec m_bufn)
 (also reset m_bufn, m_eveodd to known values now, since a transfer is done
  it also means the buffer that caused the last irq was the last to be used-
  I think this is correct, as we prevent a new 'start' if any buffer in use)
______________________________________________________________________________*/
uint16_t UsbEndptRx::check(){
    enum { U1STAT = 0xBF808640, PPBI = 1<<2 };
    bool eo = Reg::is_set8(U1STAT, PPBI);   //get PPBI
    m_count -= m_bd[eo]->count;     //subtract actual byte count received
    m_bufn--;                       //one less buffer in use
    if(m_count == 0){               //all done
        m_bufn = 0;                 //should already be 0, but now we know
        m_eveodd = eo ^ 1;          //next one to be used
        return 0;                   //done here, data is ready
    }
    if(m_count > 0){                //more to do
        rx_setup(m_count > m_max_count ? m_max_count : m_count);
    }
    return m_count;                 // >0,, not done
}

/*______________________________________________________________________________

 start a rx transfer- from buf[] for n bytes
 if rx still busy (any buffer), return false (caller tries again later)
 if started, return true
______________________________________________________________________________*/
bool UsbEndptRx::start(uint8_t* buf, uint16_t n){
    if(m_bufn || !m_max_count) return false;
    m_bufptr = Reg::k2phys((uint32_t)buf);
    m_count = n;    //store count
    uint16_t n1 = n, n2 = 0;
    if(n > m_max_count){
        n1 = m_max_count;
        n -= m_max_count;
        n2 = n > m_max_count ? m_max_count : n;
    }
    rx_setup(n1);
    if(n2) rx_setup(n2);
    return true;
}

/*______________________________________________________________________________

 setup an rx for n bytes (only called by 'check' and 'start')
 m_bufptr incremented by n, m_bufn count incremented, m_eveodd toggled
 (all for for next time)
______________________________________________________________________________*/
void UsbEndptRx::rx_setup(uint16_t n){
    m_bd[m_eveodd]->addr = m_bufptr;    //set address of buf
    m_bd[m_eveodd]->count = n;          //set count
    m_bd[m_eveodd]->uown = 1;           //give
    m_bufptr += n;                      //adjust for next time
    m_bufn++;                           //1 buffer in use
    m_eveodd ^= 1;                      //toggle even/odd
}

/*______________________________________________________________________________

 take ownership of both buffers if not already owned
 (toggle m_eveodd if we caused a 1->0)
______________________________________________________________________________*/
void UsbEndptRx::rx_stop(){
    if(m_bd[0]->uown){ m_eveodd ^= 1; m_bd[0]->uown = 0; }
    if(m_bd[1]->uown){ m_eveodd ^= 1; m_bd[1]->uown = 0; }
}







class UsbEndpt {
/*______________________________________________________________________________

    endpoint class
    handles all functions for an endpoint

    UsbEndpt ep0(0,UsbEndpt::TRX); //endpoint 0 with rx+tx
    UsbEndpt ep1rx(1,UsbEndpt::TX); //endpoint 1 with tx only
    UsbEndpt ep2tx(2,UsbEndpt::RX); //endpoint 2 with rx only
        endpoint descriptors (4) for endpoint cleared
        endpoint register cleared
        rxbuffer addresses received from UsbBuf (if endpoint has rx)
        endpoint descriptors init


    ep0.deinit(); //give up any buffers, turn off endpoint
    ep0.reinit(); //deinit, then run constructor (init)
        (cannot change endpoint number or rx/tx properties)

    ep0.on(true); //turn on endpoint with handshake
    ep0.on(false); //turn off endpoint
    ep1rx.on(ep1rx.RXEN|ep1rx.CTRLDIS); //turn on with specific options


______________________________________________________________________________*/
    public:

    //capabilities (shifted into U1EPn bit positions)
    enum TR { NONE = 0, TX = 1<<2, RX = 2<<2, TRX = 3<<2 };

    //endpoint control register bits
    enum U1EP {
        //LS = 1<<7,            /*HOST mode and U1EP0 only*/
        //RETRYDIS = 1<<6,      /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4,         /*only when TXEN=1 && RXEN=1*/
        RXEN = 1<<3, TXEN = 1<<2, ESTALL = 1<<1, HSHAKE = 1<<0
    };

    //public functions
    UsbEndpt(uint8_t, TR, uint16_t); //n, TX|RX|TRX, max size

    void deinit ();         //'destructor'-like
    void reinit ();         //deinit, then run constructor
    void on     (bool);     //enable/disable endpoint ([tx],[rx],handshake)
    void on     (uint8_t);  //enable endpoint with specified U1EP reg bits
    void token  (uint8_t);  //process transaction complete, called by isr

    //public static function
    static uint32_t bdt_addr(); //to get bdt address (for Usb::bdt_addr() use)

    //bd entry struct
//    typedef union {
//        struct { uint32_t dat; uint32_t addr; };
//        struct { unsigned :16; uint16_t count; };
//        struct {
//            unsigned :2;
//            unsigned bstall:1;
//            unsigned dts:1;
//            unsigned ninc:1;
//            unsigned keep:1;
//            unsigned data01:1;
//            unsigned uown:1;
//        };
//        struct { unsigned :2; unsigned pid:4; };
//        uint8_t control;
//        uint64_t all;
//    } bdt_t;

    private:

    //endpoint register address, spacing
    enum { U1EP0 = 0xBF808700, U1EP_SPACING = 0x10 };

    //bd entry control bits (out to usb sie)
    enum CTRLOUT {
        UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
        NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<1
    };

    //private functions
    void                bd_ctrl     (uint8_t);
    volatile uint8_t    bd_ctrl     ();
    volatile uint16_t   bd_count    ();
    void                bd_count    (uint16_t);
    volatile uint8_t    bd_pid      () const;
    void                bd_addr     (volatile uint8_t*);
    uint8_t*            bd_addr     () const;
    volatile bool       bd_uown     () const;
    volatile bool       bd_data01   () const;
    volatile bool       bd_stall    () const;
    void                bd_uown     (bool);
    void                bd_data01   (bool);
    void                bd_stall    (bool);
    void                bd_setup    (volatile uint8_t*, uint16_t, uint8_t);
    void                epreg       (U1EP, bool);
    volatile bool       epreg       (U1EP) const;
    void                epreg       (uint8_t);
    void                bd_init     ();
    void                setup_token ();
    void                in_token    ();
    void                out_token   ();
    void                rx_giveup   ();
    void                tx_cancel   ();
    void                tx_zlp      ();

    //in/out matches setup_pkt.dir- 0=from host,1=to host
    enum SETUPXFER { COMPLETE = 3, IN = 1, OUT = 0, STATUS = 2 };
    //data01 toggle bit
    enum { DATA0 = 0, DATA1 = 1<<6 /*DATA01 in CTRLOUT*/};

    static bdt_t        m_bdt[(my_last_endp+1)*4] __attribute__((aligned(512)));
    uint8_t             m_ep_n;         //0-15
    TR                  m_ep_trx;       //TX|RX|TRX (aligned for U1EPn <<2)
    volatile uint8_t*   m_ep_reg;       //U1EPn sfr register
    volatile uint8_t*   m_rxbuf[2];     //pointers to rx-even/odd buffers
    uint16_t            m_buf_len;      //fixed length from UsbBuf (64)
    uint8_t             m_tx_ep;        //current tx endpoint (2=even/3=odd)
    uint8_t             m_rx_ep;        //current rx endpoint (0=even/1=odd)
    bdt_t*              m_bd[4];        //rx/tx buffer descriptor even/odd
    UsbCh9::SetupPkt_t  m_setup_pkt;    //copy of setup data packet (8bytes)
    SETUPXFER           m_setup_stage;  //setup transaction stages
    uint8_t             m_bdi;          //current bd index (used in functions)
    uint8_t             m_idx;          //save idx (from isr call)
    uint8_t             m_data01;       //tx data0/data1, uses bit6 (toggle bit6)
    volatile uint8_t*   m_tx_ptr;       //pointer to tx data

    class UsbEndptRx    m_ep_rx;        //RX endpoint class
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//static
//UsbHandlers::attach will get bdt address, and set the address in the sfr reg
bdt_t UsbEndpt::m_bdt[(my_last_endp+1)*4] = {0};
//UsbEndpt::bdt_t UsbEndpt::m_bdt[(my_last_endp+1)*4] = {0};
uint32_t UsbEndpt::bdt_addr(){ return (uint32_t)m_bdt; }

//public
UsbEndpt::UsbEndpt(uint8_t n, TR tr, uint16_t max) :
    m_tx_ep(2),
    m_rx_ep(0),
    m_ep_n(n&15),
    m_ep_trx(tr),
    m_bd{&m_bdt[n*4],&m_bdt[n*4+1],&m_bdt[n*4+2],&m_bdt[n*4+3]},
    m_ep_reg((volatile uint8_t*)U1EP0+n*U1EP_SPACING),
    m_buf_len(UsbBuf::buf_len()),
    m_setup_pkt{0},
    m_setup_stage(COMPLETE),
    m_bdi(0),
    m_idx(0),
    m_data01(0),
    m_rxbuf{0,0},

    m_ep_rx(m_bdt, n, tr & RX ? max : 0)
{
    epreg(0);
    if(tr&RX){ m_rxbuf[0] = UsbBuf::get(); m_rxbuf[1] = UsbBuf::get(); }
    bd_init();
}
//UsbEndpt::UsbEndpt(uint8_t n, TR tr){}
void UsbEndpt::deinit(){
    epreg(0);
    UsbBuf::release(m_rxbuf[0]);
    UsbBuf::release(m_rxbuf[1]);
    tx_cancel();
}
void UsbEndpt::reinit(){ deinit(); UsbEndpt(m_ep_n,m_ep_trx, m_buf_len); }
void UsbEndpt::bd_ctrl(uint8_t v){ m_bd[m_bdi]->control = v; }
volatile uint8_t UsbEndpt::bd_ctrl(){ return m_bd[m_bdi]->control; }
volatile uint16_t UsbEndpt::bd_count(){ return m_bd[m_bdi]->count; }
void UsbEndpt::bd_count(uint16_t v){ m_bd[m_bdi]->count = v; }
volatile uint8_t UsbEndpt::bd_pid() const { return m_bd[m_bdi]->pid; }
void UsbEndpt::bd_addr(volatile uint8_t* v){
    m_bd[m_bdi]->addr = Reg::k2phys(v);
}
uint8_t* UsbEndpt::bd_addr() const {
    return (uint8_t*)Reg::p2kseg0(m_bd[m_bdi]->addr);
}
volatile bool UsbEndpt::bd_uown() const { return m_bd[m_bdi]->uown; }
volatile bool UsbEndpt::bd_data01() const { return m_bd[m_bdi]->data01; }
volatile bool UsbEndpt::bd_stall() const { return m_bd[m_bdi]->bstall; }
void UsbEndpt::bd_uown(bool tf){ m_bd[m_bdi]->uown = tf; }
void UsbEndpt::bd_data01(bool tf){ m_bd[m_bdi]->data01 = tf; }
void UsbEndpt::bd_stall(bool tf){ m_bd[m_bdi]->bstall = tf; }
void UsbEndpt::bd_setup(volatile uint8_t* a, uint16_t c, uint8_t f){
    bd_addr(a); bd_count(c); bd_ctrl(f);
}
void UsbEndpt::epreg(U1EP e, bool tf){ Reg::set(m_ep_reg, e, tf); }
volatile bool UsbEndpt::epreg(U1EP e) const {
    return Reg::is_set8(m_ep_reg, e);
}
void UsbEndpt::epreg(uint8_t v){ Reg::val8(m_ep_reg, v); }
void UsbEndpt::bd_init(){
    for(auto i = 3; i >= 0; i--){
        m_bdi = i;
        bd_setup(0, 0, 0);
        if((m_ep_trx & RX) && (i < 2)) bd_addr(m_rxbuf[i]);
    }
}
void UsbEndpt::on(bool tf){ if(tf) epreg(m_ep_trx|HSHAKE); else epreg(0); }
void UsbEndpt::on(uint8_t v){ epreg(v); }
void UsbEndpt::rx_giveup(){ bd_count(m_buf_len); bd_uown(true); }
void UsbEndpt::tx_cancel(){
    for(m_bdi = 2; m_bdi < 4; m_bdi++){
        if(bd_uown()) m_tx_ep ^= 1; //toggle, since tx uown 1->0 (?)
        bd_ctrl(0);
        UsbBuf::release(bd_addr());
    }
    UsbBuf::release(m_tx_ptr);
    m_tx_ptr = 0;
    m_bdi = m_idx; //restore
}
void UsbEndpt::tx_zlp(){
    m_bdi = m_tx_ep; //set bd to match tx endpoint
    bd_setup(0, 0, UOWN|DATA1);
    m_bdi = m_idx; //restore
}

/*..............................................................................
 process transaction complete
    called from ISR with index into this endpoint of which caused the irq
    (0-3, rx-even,rx-odd,tx-even,tx-odd)
..............................................................................*/
void UsbEndpt::token(uint8_t idx){
    Usb u; //just for style, using u. instead of Usb::

    //check if we even have an endpoint in use
    //if this is an unused endpoint below a higher used endpoint
    //turn off (should already be off, and shouldn't be here)
    if(m_ep_trx = NONE){
        on(false);
        return;
    }

    //which bd entry was used for this TRNIF
    //m_idx backup copy, when we need to temp change m_bdi
    m_bdi = m_idx = idx;

    //when tx, toggle m_tx_ep after a tx bd_setup
    //when rx, toggle m_rx_ep in setup/out

    //pid from bd entry (should only see 1,9,13)
    switch(bd_pid()){

        case UsbCh9::SETUP:
            m_setup_pkt = *(UsbCh9::SetupPkt_t*)&m_rxbuf[m_bdi]; //save data
            rx_giveup();        //done with rx buffer
            m_rx_ep ^= 1;       //rx endpoint toggled, keep track
            tx_cancel();        //anything in tx, cancel
            m_data01 = DATA1;   //first data (or status) is data1
            setup_token();      //process
            u.control(u.PKTDIS, false); //continue control transfer
            break;

        case UsbCh9::IN:
            m_tx_ep ^= 1;
            m_data01 ^= DATA01;
            in_token();
            break;

        case UsbCh9::OUT:
            m_rx_ep ^= 1; //rx endpoint toggled, keep track (?)
            out_token();
            rx_giveup(); //done with rx buffer
            break;
    }
}

/*..............................................................................
    SETUP token received (with 8byte data)
        m_setup_pkt already contains the 8bytes (copied from buffer)
        any tx was cancelled (and any buffers from UsbBuf released)
        m_tx_ptr was cleared
        m_data01 is 1
..............................................................................*/
void UsbEndpt::setup_token(){

    bool dir = m_setup_pkt.dir;
    uint8_t typ = m_setup_pkt.type;
    uint8_t recip = m_setup_pkt.recip;

    m_tx_ptr = UsbBuf::get();
    if(!m_tx_ptr){} //do something if can't get buffer- stall?

    //standard requests
    switch(m_setup_pkt.bRequest){
        case UsbCh9::CLEAR_FEATURE:
            //m_setup_pkt.wValue
            break;
        case UsbCh9::SET_FEATURE:
            //m_setup_pkt.wValue
            break;
        //case UsbCh9::SET_ADDRESS:
            //do after status
            //break;
        case UsbCh9::SET_CONFIGURATION:
            //m_setup_pkt.wValue
            break;
        case UsbCh9::GET_STATUS:
            //m_setup_pkt.wLength should = 2
            //2bytes- byte0/bit0 = self-powered?, byte0/bit1=remote wakeup?
            m_tx_ptr[0] = my_self_powered<<0;
            m_tx_ptr[0] |= my_remote_wakeup<<1;
            m_tx_ptr[1] = 0;
            break;
        case UsbCh9::GET_DESCRIPTOR:
            //
            break;
        case UsbCh9::SET_DESCRIPTOR:
            //
            break;
        case UsbCh9::GET_CONFIGURATION:
            //return 1 byte- 0=not configured
            //
            break;
    }

    m_setup_stage = (SETUPXFER)dir; //0=out,1=in
    if(m_setup_pkt.wLength == 0){   //if no data stage
        m_setup_stage = STATUS;     //in status stage
        tx_zlp();                   //set for 0byte status
    }
    else if(dir == IN){
        m_bdi = m_tx_ep; //set bd to match tx endpoint
        bd_setup(m_tx_ptr, m_setup_pkt.wLength, UOWN|m_data01);
        m_bdi = m_idx; //restore
    }

    //if no data stage,
    //  now in STATUS state and tx is ready for IN status
    //if OUT data stage is next,
    //  now in OUT state and tx is ready for IN status
    //if IN data stage, tx is ready for IN

}
/*..............................................................................

..............................................................................*/
void UsbEndpt::in_token(){
    if(m_setup_stage == STATUS){
        m_setup_stage = COMPLETE;
        UsbBuf::release(m_tx_ptr);
        m_tx_ptr = 0;
    }
    if(m_setup_stage == IN){
    //send more, or go to STATUS stage,
    //and call tx_zlp()
    }
    //if(m_setup_stage == OUT){
    //cannot get here, tx was cleared in setup, so nothing
    //is sent, hardware nacks since no tx ready
    //}

}
/*..............................................................................

..............................................................................*/
void UsbEndpt::out_token(){
    if(m_setup_stage == STATUS){
        m_setup_stage = COMPLETE;
        UsbBuf::release(m_tx_ptr);
        m_tx_ptr = 0;
        return;
    }
    if(m_setup_stage == OUT){
    //rx more, or go to STATUS stage,
    //and call tx_zlp()
    }
    if(m_setup_stage == IN){
    //problem, was expecting an IN -stall
    }


}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/*..............................................................................

..............................................................................*/
