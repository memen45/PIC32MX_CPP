#pragma once

#include "Usb.hpp"
#include "Reg.hpp"


/*______________________________________________________________________________

 using .hpp mostly for now


 common typedefs
 (move to better spot later)
______________________________________________________________________________*/
typedef union {
    struct {
        unsigned :2;
        unsigned bstall:1;
        unsigned dts:1;
        unsigned ninc:1;
        unsigned keep:1;
        unsigned data01:1;
        unsigned uown:1;
        unsigned :8;
        unsigned count:10;
        unsigned :6;
        unsigned addr:32;
    };
    struct { unsigned :2; unsigned pid:4; };
    uint8_t ctrl;
    uint64_t all;
} bdt_t;




class UsbEndptTRx {
/*______________________________________________________________________________

 RX/TX endpoint class
    handles only RX or TX endpoints in specific endpoint
    UsbEndpt uses this class for RX or TX operations
    UsbEndpt in charge of U1EPn

    constructor needs the bdt address of endpoint n (and+1 if tx),
    and max packet size
    (if max packet size set to 0, this RX or TX is not used-
     start() will always return false)

    reinit()
        -clears all private data, bd entries (also used in constructor)
        -no return value

    options(opt)
        -sets bd control bits to use other than UOWN and DATA01
        -(KEEP, NINC, DTS, BSTALL), use correct bit positions

    start(*buf,count,[d01=0],[bstall=0])
        -starts a rx into buf or tx from buf, for count bytes
        -optionally set data01 (default is data0)
        -optionally set bstall (default 0)
        -returns true if started, false if cannot (any buffer busy)

    check()
        -after an IN/OUT/SETUP irq, checks if any more data to receive or
         send from previous start()
        -if not complete, returns bytes left to rx or tx
        -if completed, returns 0

    stop()
        -take back ownership of rx or tx endpoint (both even,odd)


    to ready endpoint 0 for a setup (in UsbEndpt::)-
        m_ep_rx.start(my_buf, 8);
        //in token check code for setup
        if(m_ep_rx.check() == 0){ we have a setup packet in my_buf }
        //or maybe just
        m_ep_check(); //since we have a setup transaction completed- must be ok


______________________________________________________________________________*/

    public:
                UsbEndptTRx (bdt_t*, uint16_t);
    void        reinit      ();
    void        options     (uint8_t);
    bool        start       (uint8_t*, uint16_t, bool = 0, bool = 0);
    uint16_t    check       ();
    void        stop        ();
    bool        stalled     ();



    private:

    void        setup       (uint16_t n);

    bdt_t* const        m_bd[2];        //even/odd bd entry pointers
    uint8_t             m_options;      //extra control options (not UOWN)
    const uint16_t      m_max_count;    //max data packet size
    uint32_t            m_bufptr;       //current buffer pointer (phys address)
    uint8_t             m_bufn;         //how many buffers in use (0,1,2)
    bool                m_eveodd;       //even/odd entry to use next
    uint16_t            m_count;        //keep track of rx count
                                        //uint16_t so can do 0-1023
};

/*______________________________________________________________________________

 init (constructor) or reinit rx/tx endpoint
 set bd rx/tx addresses (even/odd) of endpoint n (constructor only)
 (passed in via bdt_t* which points to position of rx or tx of endpoint n)
 also need to know max packet size (constructor only)
 all vars reset, bd entries cleared
______________________________________________________________________________*/
UsbEndptTRx::UsbEndptTRx(bdt_t* b, uint16_t max)
    : m_bd{b, b+1}, m_max_count(max)
{
    reinit();
}
void UsbEndptTRx::reinit(){
    m_options = 0;
    m_bufptr = 0;
    m_bufn = 0;
    m_eveodd = 0;
    m_count = 0;
    m_bd[0]->all = 0;
    m_bd[1]->all = 0;
}

/*______________________________________________________________________________

 set DATA01, KEEP, NINC, DTS option(s)
 (provide bits in correct bit position)
 BSTALL, UOWN unchanged
______________________________________________________________________________*/
void UsbEndptTRx::options(uint8_t v){
    m_options = (v & 0x78); //bits x6543xxx
}

/*______________________________________________________________________________

 check rx/tx status- return bytes remaining, or 0 if done
 called only from UsbEndpt:: when TRNIF and token type is IN (for TX) or
 OUT/SETUP (for RX)
 use U1STAT to check which buffer (even/odd) caused the irq
 (must be a buffer returned back to us from usb, so dec m_bufn)
 (if done, also reset m_bufn, m_eveodd to known values now, since a transfer
  is done, it also means the buffer that caused the last irq was the last to be
  used- I think this is correct, as we prevent a new 'start' if any buffer in
  use)
 (if host sends a less than full packet AND is now less than expected number
  of bytes received, host shorted us and is done, if we sent less than full
  packet for some reason AND still remaining bytes to send, we shorted the
  host- either case is considered done here)
______________________________________________________________________________*/
uint16_t UsbEndptTRx::check(){
    enum { U1STAT = 0xBF808640, PPBI = 1<<2 };
    bool eo = Reg::is_set8(U1STAT, PPBI);   //get PPBI
    uint16_t c = m_bd[eo]->count;   //get actual count rx/tx
    m_count -= c;                   //subtract from saved count
    m_bufn--;                       //one less buffer in use
    if(m_count == 0 || c < m_max_count){ //0 = all done, c < max =short packet
        m_count = 0;                //clear m_count in case we were shorted,
        m_bufn = 0;                 //should already be 0, but now we know
        m_eveodd = eo ^ 1;          //next one to be used is not this one
    } else {
        setup(m_count > m_max_count ? m_max_count : m_count);
    }
    return m_count;                 // 0=done, >0=not done
}

/*______________________________________________________________________________

 start a rx/tx transfer- from buf[] for n bytes, [d01 = 0], [bstall = 0]
 if rx/tx still busy (any buffer) or stalled, return false
 (caller tries again later), else return true
 (if RX/TX not used, m_max_count will be 0- return false always)
 if n > max packet size, also get second buffer ready now
______________________________________________________________________________*/
bool UsbEndptTRx::start(uint8_t* buf, uint16_t n, bool d01, bool bstall){
    if(m_bufn || m_max_count == 0 || stalled()) return false;
    m_options &= ~(1<<6);
    m_options |= (d01<<6)|(bstall<<2);
    m_bufptr = Reg::k2phys((uint32_t)buf);
    m_count = n;
    uint16_t n1 = n, n2 = 0;
    if(n > m_max_count){
        n1 = m_max_count;
        n -= m_max_count;
        n2 = n > m_max_count ? m_max_count : n;
    }
    setup(n1);
    if(n2 && bstall == 0) setup(n2);
    m_options &= ~(1<<2); //clear bstall in m_options, only needed temporarily
    return true;
}

/*______________________________________________________________________________

 setup an rx/tx for n bytes (called by start() and check() )
 m_bufptr incremented by n, m_bufn count incremented, m_eveodd toggled,
 m_options bit6 (data01) toggled (all for for next time)
______________________________________________________________________________*/
void UsbEndptTRx::setup(uint16_t n){
    m_bd[m_eveodd]->addr = m_bufptr;    //set address of buf
    m_bd[m_eveodd]->count = n;          //set count
    m_bd[m_eveodd]->ctrl = m_options;   //options other than UOWN and BSTALL
    m_bd[m_eveodd]->uown = 1;           //give to usb hardware
    m_bufptr += n;                      //adjust for next time
    m_bufn++;                           //1 buffer in use
    m_eveodd ^= 1;                      //toggle even/odd
    m_options ^= 1<<6;                  //toggle data01
}

/*______________________________________________________________________________

 take ownership of both buffers if not already owned
 (toggle m_eveodd if we caused a 1->0)
______________________________________________________________________________*/
void UsbEndptTRx::stop(){
    if(m_bd[0]->uown){ m_eveodd ^= 1; m_bd[0]->uown = 0; }
    if(m_bd[1]->uown){ m_eveodd ^= 1; m_bd[1]->uown = 0; }
}

/*______________________________________________________________________________

 get stall status (stalled if either buffer stalled)
 if a setup packet received on this endpoint, bstall automatically cleared
______________________________________________________________________________*/
bool UsbEndptTRx::stalled(){
    return ((m_bd[0]->uown && m_bd[0]->bstall) ||
             m_bd[1]->uown && m_bd[1]->bstall);
}















class UsbEndpt {
/*______________________________________________________________________________

    endpoint class
    handles all functions for an endpoint

    UsbEndpt ep0(0, UsbEndpt::TRX, 64);  //endpoint 0 with rx+tx, 64byte max
    UsbEndpt ep1rx(1, UsbEndpt::TX, 64); //endpoint 1 with tx only, 64byte max
    UsbEndpt ep2tx(2, UsbEndpt::RX, 64); //endpoint 2 with rx only, 64byte max
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
////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

    private:

    //endpoint register address, spacing
    enum { U1EP0 = 0xBF808700, U1EP_SPACING = 0x10 };

    //bd entry control bits (out to usb sie)
    enum CTRLOUT {
        UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
        NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<1
    };

    //private functions
    void                epreg       (U1EP, bool);
    volatile bool       epreg       (U1EP) const;
    void                epreg       (uint8_t);
    void                setup_token ();
    void                in_token    ();
    void                out_token   ();


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


    bdt_t*              m_bd[4];        //rx/tx buffer descriptor even/odd
    UsbCh9::SetupPkt_t  m_setup_pkt;    //copy of setup data packet (8bytes)
    SETUPXFER           m_setup_stage;  //setup transaction stages

    volatile uint8_t*   m_tx_ptr;       //pointer to tx data

    class UsbEndptTRx    m_RX;          //RX endpoint class
    class UsbEndptTRx    m_TX;          //TX endpoint class
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//static
//UsbHandlers::attach will get bdt address, and set the address in the sfr reg
bdt_t UsbEndpt::m_bdt[(my_last_endp+1)*4] = {0};
uint32_t UsbEndpt::bdt_addr(){ return (uint32_t)m_bdt; }

//public
UsbEndpt::UsbEndpt(uint8_t n, TR tr, uint16_t max) :
    m_ep_n(n&15),
    m_ep_trx(tr),
    m_bd{&m_bdt[n*4],&m_bdt[n*4+1],&m_bdt[n*4+2],&m_bdt[n*4+3]},
    m_ep_reg((volatile uint8_t*)U1EP0+n*U1EP_SPACING),
    m_buf_len(UsbBuf::buf_len()),
    m_setup_pkt{0},
    m_setup_stage(COMPLETE),
    m_rxbuf{tr&RX?UsbBuf::get():0,tr&RX?UsbBuf::get():0},
    m_RX(&m_bdt[n*4], tr & RX ? max : 0),
    m_TX(&m_bdt[n*4+2], tr & TX ? max : 0)
{
    epreg(0);
}
//UsbEndpt::UsbEndpt(uint8_t n, TR tr){}
void UsbEndpt::deinit(){
    epreg(0);
    UsbBuf::release(m_rxbuf[0]);
    UsbBuf::release(m_rxbuf[1]);
    m_TX.stop();
    m_RX.stop();
}
void UsbEndpt::reinit(){
    deinit();
    UsbEndpt(m_ep_n,m_ep_trx, m_buf_len);
    m_RX.reinit();
    m_TX.reinit();
}
void UsbEndpt::epreg(U1EP e, bool tf){
    Reg::set(m_ep_reg, e, tf);
}
volatile bool UsbEndpt::epreg(U1EP e) const {
    return Reg::is_set8(m_ep_reg, e);
}
void UsbEndpt::epreg(uint8_t v){
    Reg::val8(m_ep_reg, v);
}
void UsbEndpt::on(bool tf){
    if(tf) epreg(m_ep_trx|HSHAKE);
    else epreg(0);
}
void UsbEndpt::on(uint8_t v){
    epreg(v);
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

    //pid from bd entry (should only see 1,9,13)
    switch(m_bd[idx]->pid){

        case UsbCh9::SETUP:
            //m_setup_pkt

            m_TX.stop();        //anything in tx, cancel

            setup_token();      //process
            u.control(u.PKTDIS, false); //continue control transfer
            break;

        case UsbCh9::IN:

            in_token();
            break;

        case UsbCh9::OUT:

            out_token();

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
        m_TX.start(0,0,1);          //set for 0byte status
    } else if(dir == IN) {
        //need to check buffer size vs wLength
        m_TX.start((uint8_t*)m_tx_ptr, m_setup_pkt.wLength, 1);
    } else {
        //need to check buffer size vs wLength
        m_RX.start((uint8_t*)m_rxbuf[0], m_setup_pkt.wLength, 1);
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
