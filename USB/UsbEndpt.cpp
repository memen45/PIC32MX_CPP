#include "UsbEndpt.hpp"
#include "Reg.hpp"
#include "UsbConfig.hpp"

enum { U1STAT = 0xBF808640, PPBI = 1<<2 };

/*______________________________________________________________________________

 init (constructor) or reinit rx/tx endpoint
 set bd rx/tx addresses (even/odd) of endpoint n (constructor only)
 (passed in via bdt_t* which points to position of rx or tx of endpoint n)
 also need to know max packet size (constructor only)
 all vars reset, bd entries cleared
______________________________________________________________________________*/
//=============================================================================
UsbEndptTRx::UsbEndptTRx(bdt_t* b, uint16_t max)
//=============================================================================
    : m_bd{b, b+1}, m_max_count(max)
{
    reinit();
}

//=============================================================================
void UsbEndptTRx::reinit()
//=============================================================================
{
    m_options = 0;
    m_bufptr = 0;
    m_bufn = 0;
    m_eveodd = 0;
    m_count = 0;
    m_trx_count = 0;
    m_bd[0]->all = 0;
    m_bd[1]->all = 0;
}

/*______________________________________________________________________________

 set DATA01, KEEP, NINC, DTS option(s)
 (provide bits in correct bit position)
 BSTALL, UOWN unchanged
______________________________________________________________________________*/
//=============================================================================
void UsbEndptTRx::options(uint8_t v)
//=============================================================================
{
    m_options = (v bitand 0x78); //bits x6543xxx
}

/*______________________________________________________________________________

 check rx/tx status- return actual bytes tx/rx, or 0 if not done
 called only from UsbEndpt:: when TRNIF
 use U1STAT to check which buffer (even/odd) caused the irq
 (must be a buffer returned back to us from usb, so dec m_bufn)
 (if done, also reset m_bufn, m_eveodd to known values now, since a transfer
  is done, it also means the buffer that caused the last irq was the last to be
  used- I think this is correct, as we prevent a new 'start' if any buffer in
  use)
 (if host sends a less than full packet AND is now less than expected number
  of bytes received, host shorted us and is done, if we sent less than full
  packet for some reason AND still remaining bytes to send, we shorted the
  host- either case is considered done here and return actual tx/rx count)
______________________________________________________________________________*/
//=============================================================================
uint16_t UsbEndptTRx::check()
//=============================================================================
{
    bool eo = Reg::anybit(U1STAT, PPBI);//get PPBI
    bdt_t& bd = *m_bd[eo];          //references look nicer to user
    uint16_t c = bd.count;          //get actual count rx/tx
    m_trx_count += c;               //add to total
    m_bufn--;                       //one less buffer in use
    //all done if >= requested or if c < max (short packet)
    if(m_trx_count >= m_count or c < m_max_count){
        m_bufn = 0;                 //should already be 0, but now we know
        m_eveodd = eo xor 1;        //next one to be used is not this one
        return m_trx_count;         //return actual count
    }
    //more, calculate remaining
    uint16_t rem = m_count - m_trx_count;
    setup(rem > m_max_count ? m_max_count : rem);
    return 0;                       // 0= not done
}

/*______________________________________________________________________________

 start a rx/tx transfer- from buf[] for n bytes, [d01 = 0], [bstall = 0]
 if rx/tx still busy (any buffer) or stalled, return false
 (caller tries again later), else return true
 (if RX/TX not used, m_max_count will be 0- return false always)
 if n > max packet size, also get second buffer ready now
______________________________________________________________________________*/
//=============================================================================
bool UsbEndptTRx::start(uint8_t* buf, uint16_t n, bool d01, bool bstall)
//=============================================================================
{
    if(m_bufn or m_max_count == 0 or stalled()) return false;
    m_options and_eq compl(1<<6);
    m_options or_eq (d01<<6)|(bstall<<2);
    m_bufptr = Reg::k2phys((uint32_t)buf);
    m_count = n;
    m_trx_count = 0;
    uint16_t n1 = n, n2 = 0;
    if(n > m_max_count){
        n1 = m_max_count;
        n -= m_max_count;
        n2 = n > m_max_count ? m_max_count : n;
    }
    setup(n1);
    if(n2 and bstall == 0) setup(n2);
    m_options and_eq compl(1<<2); //clear bstall in m_options, temporarily
    return true;
}

/*______________________________________________________________________________

 setup an rx/tx for n bytes (called by start() and check() )
 m_bufptr incremented by n, m_bufn count incremented, m_eveodd toggled,
 m_options bit6 (data01) toggled (all for for next time)
______________________________________________________________________________*/
//=============================================================================
void UsbEndptTRx::setup(uint16_t n)
//=============================================================================
{
    bdt_t& bd = *m_bd[m_eveodd];        //get reference, so eveodd does not
                                        //have to compute on every one
    bd.addr = m_bufptr;                 //set address of buf
    bd.count = n;                       //set count
    bd.ctrl = m_options;                //options other than UOWN and BSTALL
    bd.uown = 1;                        //give to usb hardware
    m_bufptr += n;                      //adjust for next time
    m_bufn++;                           //1 buffer now in use
    m_eveodd xor_eq 1;                  //toggle even/odd
    m_options xor_eq 1<<6;              //toggle data01
}

/*______________________________________________________________________________

 take ownership of both buffers if not already owned
 (toggle m_eveodd if we caused a 1->0)
______________________________________________________________________________*/
//=============================================================================
void UsbEndptTRx::stop()
//=============================================================================
{
    bdt_t& bde = *m_bd[0]; bdt_t& bdo = *m_bd[1];
    if(bde.uown){ m_eveodd xor_eq 1; bde.uown = 0; }
    if(bdo.uown){ m_eveodd xor_eq 1; bdo.uown = 0; }
}

/*______________________________________________________________________________

 get stall status (stalled if either buffer stalled)
 if a setup packet received on this endpoint, bstall automatically cleared
______________________________________________________________________________*/
//=============================================================================
bool UsbEndptTRx::stalled()
//=============================================================================
{
    bdt_t& bde = *m_bd[0]; bdt_t& bdo = *m_bd[1];
    return ((bde.uown and bde.bstall) or (bdo.uown and bdo.bstall));
}









//endpoint register address, spacing
enum { U1EP0 = 0xBF808700, U1EP_SPACING = 0x10 };


//static
//UsbHandlers::attach will get bdt address, and set the address in the sfr reg
UsbEndptTRx::bdt_t UsbEndpt::m_bdt[(UsbConfig::last_endp+1)*4] = {0};
uint32_t UsbEndpt::bdt_addr(){ return (uint32_t)m_bdt; }

//public
//=============================================================================
UsbEndpt::UsbEndpt(uint8_t n, TR tr, uint16_t max) :
//=============================================================================
    m_ep_n(n&15),
    m_ep_trx(tr),
    m_bd{&m_bdt[n*4],&m_bdt[n*4+1],&m_bdt[n*4+2],&m_bdt[n*4+3]},
    m_ep_reg((volatile uint8_t*)U1EP0+n*U1EP_SPACING),
    m_setup_pkt{0},
    m_setup_stage(COMPLETE),
    m_rxbuf{tr&RX?UsbBuf::get64():0,tr&RX?UsbBuf::get64():0},
    m_RX(&m_bdt[n*4], tr bitand RX ? max : 0),
    m_TX(&m_bdt[n*4+2], tr bitand TX ? max : 0)
{
    epreg(0);
}


//UsbEndpt::UsbEndpt(uint8_t n, TR tr){}
//=============================================================================
void UsbEndpt::deinit()
//=============================================================================
{
    epreg(0);
    UsbBuf::release((uint8_t*)m_rxbuf[0]);
    UsbBuf::release((uint8_t*)m_rxbuf[1]);
    m_TX.stop();
    m_RX.stop();
}

//=============================================================================
void UsbEndpt::reinit()
//=============================================================================
{
    deinit();
    UsbEndpt(m_ep_n,m_ep_trx, 64);
    m_RX.reinit();
    m_TX.reinit();
}

//=============================================================================
void UsbEndpt::epreg(U1EP e, bool tf)
//=============================================================================
{
    Reg::setbit(m_ep_reg, e, tf);
}

//=============================================================================
volatile bool UsbEndpt::epreg(U1EP e) const
//=============================================================================
{
    return Reg::anybit(m_ep_reg, e);
}

//=============================================================================
void UsbEndpt::epreg(uint8_t v)
//=============================================================================
{
    Reg::val(m_ep_reg, v);
}

//=============================================================================
void UsbEndpt::on(bool tf)
//=============================================================================
{
    if(tf) epreg(m_ep_trx|HSHAKE);
    else epreg(0);
}

//=============================================================================
void UsbEndpt::on(uint8_t v)
//=============================================================================
{
    epreg(v);
}


/*..............................................................................
 process transaction complete
    called from ISR with index into this endpoint of which caused the irq
    (0-3, rx-even,rx-odd,tx-even,tx-odd)
..............................................................................*/
//=============================================================================
void UsbEndpt::token(uint8_t idx)
//=============================================================================
{
    Usb u; //just for style, using u. instead of Usb::

    //check if we even have an endpoint in use
    //if this is an unused endpoint below a higher used endpoint
    //turn off (should already be off, and shouldn't be here)
    if(m_ep_trx == NONE){
        on(false);
        return;
    }

    //which bd entry was used for this TRNIF

    //pid from bd entry (should only see 1,9,13)
    switch(m_bd[idx]->pid){

        case UsbCh9::SETUP:
            m_TX.stop();            //anything in tx, cancel
            if(m_RX.check() not_eq 8){} //something wrong, assume ok for now
            setup_token();          //process m_setup_pkt
            u.control(u.PKTDIS, false); //and continue control transfer
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
        m_setup_pkt already contains the 8bytes
        any tx was cancelled
..............................................................................*/
//=============================================================================
void UsbEndpt::setup_token()
//=============================================================================
{

    bool dir = m_setup_pkt.dir;
    //uint8_t typ = m_setup_pkt.type;
    //uint8_t recip = m_setup_pkt.recip;

    m_tx_ptr = UsbBuf::get64();
    if(not m_tx_ptr){} //do something if can't get buffer- stall?

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
            m_tx_ptr->buf[0] = UsbConfig::self_powered<<0;
            m_tx_ptr->buf[0] or_eq UsbConfig::remote_wakeup<<1;
            m_tx_ptr->buf[1] = 0;
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
        //assume <= 64 for now
        m_TX.start((uint8_t*)m_tx_ptr, m_setup_pkt.wLength, 1); //d01=1
    } else {
        //need to check buffer size vs wLength
        //assume <= 64 for now
        m_RX.start((uint8_t*)m_rxbuf[0], m_setup_pkt.wLength, 1); //d01=1
    }

    //if no data stage,
    //  now in STATUS state and tx is ready for IN status
    //if OUT data stage is next,
    //  now in OUT state and tx is ready for IN status
    //if IN data stage, tx is ready for IN

}

//=============================================================================
void UsbEndpt::in_token()
//=============================================================================
{
    if(m_setup_stage == STATUS){
        m_setup_stage = COMPLETE;
        UsbBuf::release((uint8_t*)m_tx_ptr);
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

//=============================================================================
void UsbEndpt::out_token()
//=============================================================================
{
    if(m_setup_stage == STATUS){
        m_setup_stage = COMPLETE;
        UsbBuf::release((uint8_t*)m_tx_ptr);
        m_tx_ptr = 0;
        return;
    }
    if(m_setup_stage == OUT){
    //rx more, or go to STATUS stage,
    //(tx already set for STATUS)
    }
    if(m_setup_stage == IN){
    //problem, was expecting an IN -stall
    }


}


