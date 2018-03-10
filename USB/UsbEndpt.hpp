#pragma once

#include "Usb.hpp"
#include "UsbBuf.hpp"


struct UsbEndptTRx {
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
        -if not complete, returns 0
        -if completed, returns number of bytes tx/rx (could be less than wanted)

    stop()
        -take back ownership of rx or tx endpoint (both even,odd)


    to ready endpoint 0 for a setup (in UsbEndpt::)-
        m_ep_rx.start(my_buf, 8);
        //in token check code for setup
        if(m_ep_rx.check() == 0){ we have a setup packet in my_buf }
        //or maybe just
        m_ep_check(); //since we have a setup transaction completed- must be ok


______________________________________________________________________________*/

    using bdt_t = union {
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
    };

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
    uint16_t            m_count;        //requested transfer count
    uint16_t            m_trx_count;    //actual transfer count (could be less)
};











struct UsbEndpt {
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


    //capabilities (shifted into U1EPn bit positions)
    enum TR { NONE = 0, TX = 1<<2, RX = 2<<2, TRX = 3<<2 };

    //endpoint control register bits
    enum U1EP {
        //LS = 1<<7,            /*HOST mode and U1EP0 only*/
        //RETRYDIS = 1<<6,      /*HOST mode and U1EP0 only*/
        CTRLDIS = 1<<4,         /*only when TXEN=1 and RXEN=1*/
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

    private:

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

    static UsbEndptTRx::bdt_t m_bdt[] __attribute__((aligned(512)));

    uint8_t             m_ep_n;         //0-15
    TR                  m_ep_trx;       //TX|RX|TRX (aligned for U1EPn <<2)
    UsbEndptTRx::bdt_t* m_bd[4];        //rx/tx buffer descriptor even/odd
    volatile uint8_t*   m_ep_reg;       //U1EPn sfr register
    UsbCh9::SetupPkt_t  m_setup_pkt;    //copy of setup data packet (8bytes)
    SETUPXFER           m_setup_stage;  //setup transaction stages
    UsbBuf::buffer64_t* m_rxbuf[2];     //pointers to rx-even/odd buffers


    UsbBuf::buffer64_t* m_tx_ptr;       //pointer to tx data

    class UsbEndptTRx   m_RX;           //RX endpoint class
    class UsbEndptTRx   m_TX;           //TX endpoint class
};
