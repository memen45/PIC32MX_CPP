#include "UsbEP2.hpp"
#include "UsbBdt.hpp"
#include "UsbCh9.hpp"
#include "Usb.hpp"

#include <cstdint>
#include <cstring> //memset, memcpy

//this file only, for ep0
static uint8_t ep0rxbuf[64] = {0};
static uint8_t pkt[8] = {0};
static uint8_t* ep0txbuf;
static bool control (UsbEP*);
static bool set_address(UsbEP*);
//

//=============================================================================
    void    UsbEP::reset            (TXRX trx, bool saveppbi)
//=============================================================================
{
    info_t& x = m_ep[trx];
    bool ppbi = saveppbi ? x.ppbi : 0;
    x = {0};
    x.ppbi = ppbi;
    x.bdt = &UsbBdt::table[m_epnum<<2 bitor trx<<1];
}

//=============================================================================
    bool    UsbEP::init             (uint8_t n, uint16_t rxsiz, uint16_t txsiz)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num+1) return false;
    m_epnum = n;
    m_rx.epsiz = rxsiz;
    m_tx.epsiz = txsiz;
    reset(TX);
    reset(RX);
    if(n) return true;
    //ep0
    return xfer(RX, ep0rxbuf, siz);
}

//=============================================================================
    bool    UsbEP::set_buf          (TXRX trx, uint8_t* buf, uint16_t siz)
//=============================================================================
{
    m_ep[trx].buf = buf;
    m_ep[trx].siz = siz;
}

//=============================================================================
    bool    UsbEP::set_notify       (TXRX trx, notify_t f)
//=============================================================================
{
    m_ep[trx].notify = f;
}

//specify d01, notify optional (default = 0)
//=============================================================================
bool UsbEP::xfer(TXRX trx, uint8_t* buf, uint16_t siz, bool d01, notify_t f)
//=============================================================================
{
    m_ep[trx].d01 = d01;
    return recv(trx, buf, siz, f);
}

//=============================================================================
bool UsbEP::xfer(TXRX trx, uint8_t* buf, uint16_t siz, notify_t f)
//=============================================================================
{
    info_t& x = m_ep[trx];
    x.buf = buf;
    x.siz = siz;
    x.btogo = siz;
    x.bdone = 0;
    x.notify = f;
    return setup(trx);
}

//=============================================================================
    bool    UsbEP::busy                 (TXRX trx)
//=============================================================================
{
    info_t& x = m_ep[trx];
    return x.bdt[0].stat.uown or x.bdt[1].stat.uown;
}

//=============================================================================
    bool    UsbEP::setup                (TXRX trx, bool stall)
//=============================================================================
{
    info_t& x = m_ep[trx];
    if(not x.buf) return false;//no buffer set
    bool i = x.ppbi;
    if(x.bdt[i].stat.uown) i xor_eq 1;      //in use, try next
    if(x.bdt[i].stat.uown) return false;    //both in use

    volatile UsbBdt::ctrl_t ctrl = {0};
    ctrl.bstall = stall;
    ctrl.data01 = x.d01;
    ctrl.uown = 1;
    ctrl.dts = 1;
    ctrl.count = x.btogo < 64 ? x.btogo : 64;
    x.bdt[i].bufaddr = Reg::k2phys((uint32_t)x.buf);
    x.bdt[i].ctrl.val32 = ctrl.val32;
    return true;
}

//called from ISR with rx/tx,even/odd (0-3)
//=============================================================================
    bool    UsbEP::service              (uint8_t ustat)
//=============================================================================
{
    m_ustat = ustat bitand 3;
    info_t& x = m_ep[ustat>>1]; //tx or rx
    x.stat = x.bdt.stat;        //get bdt stat
    x.ppbi xor_eq 1;            //toggle ppbi
    x.d01 xor_eq 1;             //toggle d01

    //in (tx), out (rx)
    switch(x.stat.pid){
        case UsbCh9::OUT:
            rxout();
            break;
        case UsbCh9::IN:
            txin();
            break;
        case UsbCh9::SETUP:
            if(m_epnum == 0 and x.stat.count == 8){
                //make copy of setup packet
                memcpy(pkt, m_rx_buf, sizeof(pkt);
                control(this);
                //clear PKTDIS to let control xfer resume
                Usb::control(Usb::PKTDIS, false);
                //rx,tx setup by control()
            }
            break;
        default:
            return false;
    }
    return true;
}

//=============================================================================
    bool    UsbEP::takeback             (TXRX trx)
//=============================================================================
{
    info_t& x = m_ep[trx];
    if(x.bdt[0].stat.uown){ //check even
        x.bdt[0].uown = 0;
        x.ppbi xor_eq 1;
    }
    if(x.bdt[1].stat.uown){ //check odd
        x.bdt[1].uown = 0;
        x.ppbi xor_eq 1;
    }
    reset(trx, true); //reset, but save ppbi
}
//=============================================================================
    void    UsbEP::txzlp                ()
//=============================================================================
{
    m_tx.zlp = true;
}
//=============================================================================
    void    UsbEP::txin                 ()
//=============================================================================
{
    m_tx.bdone += m_tx.stat.count;
    if(m_tx.stat.count > m_tx.btogo){
        m_tx.btogo = 0;
    } else m_tx.btogo -= m_tx.stat.count;

    if(m_tx.btogo){
        m_tx.buf += m_tx.stat.count;
        setup(TX);
    } else if(m_tx.zlp){
        m_tx.zlp = 0;
        setup(TX);
    } else if(m_tx.notify){
        m_tx.notify(this);
        m_tx.notify = 0;
    }

    if(m_epnum == 0 and m_tx.btogo == 0) UsbBuf::release(ep0txbuf);
}

//=============================================================================
    void    UsbEP::rxout                ()
//=============================================================================
{
    m_rx.bdone += m_rx.stat.count;

    if(not m_rx.stat.count or           //zlp
        m_rx.stat.count > m_rx.btogo or //rx more than planned
        m_rx.stat.count < m_rx.epsiz){  //short packet
        m_rx.btogo = 0;                 //no more to do
    } else m_rx.btogo -= m_rx.stat.count;

    if(m_rx.btogo){
        m_rx.buf += m_rx.stat.count;
        setup(RX);
    } else if(m_rx.notify){
        m_rx.notify(this);
        m_rx.notify = 0;
    }
}



//this file only- for ep0 only
//=============================================================================
static bool control (UsbEP* ep)
//=============================================================================
{
    UsbConfig ucfg; Usb usb;

    //setup packet is now in pkt
    //take back any pending tx on ep0
    ep->takeback(ep->TX);
    //in all cases after a setup, next tx is data1 for either data or status
    ep->m_tx.d01 = 1;
    //assume rx needs to be set for next setup (change if need rx dat phase)
    ep->m_rx.d01 = 0;

    //release any previous tx
    UsbBuf::release(ep0txbuf);

    //get a tx buffer
    ep0txbuf = UsbBuf::get64();
    if(not ep0txbuf){
        return false; //could not get a buffer (unlikely)
    }

    //preset first 2 bytes
    ep0txbuf[0] = 0;
    ep0txbuf[1] = 0;

    //assume tx (IN) length requested
    uint16_t tlen = pkt.wLength;
    //assume rx (OUT) status
    uint16_t rlen = 0;

    //process std req
    //pkt.wRequest, wValue, wIndex, wLength

    switch(pkt.wRequest){

        case UsbCh9::DEV_GET_STATUS:
            //setup (here), tx 2bytes (data1), status (rx zlp data1)
            //tlen should be 2
            ep0txbuf[0] = ucfg.self_powered bitor (ucfg.remote_wakeup<<1);
            break;
        case UsbCh9::IF_GET_STATUS:
            //setup (here), tx 2bytes 0x00 0x00 (data1), status (rx zlp data1)
            //tlen should be 2
            //already setup
            break;
        case UsbCh9::EP_GET_STATUS:
            //setup (here), tx 2bytes (data1), status (rx zlp data1)
            //tlen should be 2
            ep0txbuf[0] = usb.epcontrol(pkt.wIndex, usb.EPSTALL);
            break;

        case UsbCh9::DEV_CLEAR_FEATURE:
            //setup (here), no data, status (tx zlp)
            //remote wakup, test mode, ignore both
            //let status proceed
            //tlen should be 0
            break;

        case UsbCh9::EP_CLEAR_FEATURE:
            //setup (here), no data, status (tx zlp)
            // only option is ENDPOINT_HALT (0x00) (not for ep0)
            //tlen should be 0
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, false);
            }
            break;

        case UsbCh9::DEV_SET_FEATURE:
            //setup (here), no data, status (tx zlp)
            //remote wakup, test mode, ignore both
            //let status proceed
            //tlen should be 0
            break;

        case UsbCh9::EP_SET_FEATURE:
            //setup (here), no data, status (tx zlp)
            //only option is ENDPOINT_HALT (0x00) (not for ep0)
            //tlen should be 0
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, true);
            }
            break;

        case UsbCh9::DEV_SET_ADDRESS:
            //setup (here), no data, status (tx zlp)
            //set address via tx callback
            //tlen should be 0
            ep->set_notify(ep->TX, set_address);
            break;

        case UsbCh9::DEV_GET_DESCRIPTOR:
            //setup (here), tx tlen bytes (data1), status (rx zlp data1)
            uint8_t* sbuf;
            sbuf = UsbDescriptors::get(pkt.wValue, &tlen);
            if(not sbuf) break; //tlen is also 0
            UsbBuf::release(ep0txbuf);
            ep0txbuf = sbuf;
            break;

        case UsbCh9::DEV_SET_DESCRIPTOR:
            //setup (here), no data, status (tx zlp)
            //tlen should be 0
            break;

        case UsbCh9::DEV_GET_CONFIGURATION:
            //setup (here), tx tlen bytes (data1), status (rx zlp data1)
            //send 0=not configured, anything else=configured
            ep0txbuf[0] = 1; //TODO
            break;

        case UsbCh9::DEV_SET_CONFIGURATION:
            //setup (here), no data, status (tx zlp)
            //let status proceed
            //tlen should be 0
            break;

        case UsbCh9::IF_GET_IFACE:
            break;
        case UsbCh9::IF_SET_IFACE:
            break;
        case UsbCh9::EP_SYNC_HFRAME:
            break;

        case UsbCh9::CDC_SET_LINE_CODING:
            //setup (here), rx data1, status (tx zlp)
            rlen = pkt.wLength; //7
            tlen = 0;
            break;
        case UsbCh9::CDC_GET_LINE_CODING:
            //setup (here), tx data1, status (rx zlp)
            break;
        case UsbCh9::CDC_SET_CONTROL_LINE_STATE:
            //setup (here), no data, status (tx zlp)
            break;

        default:
            //check other requests
            break;
    }

    //check if no data for request (bad string index, etc)
    if((tlen == 0) and (rlen == 0) and pkt.wLength){
        //stall
    }
    //check for tx data that is less than requested (strings,descriptors,etc)
    else if(tlen and (tlen != pkt.wLength)){
        if(not (tlen % 64)) ep->txzlp(); //add zlp if same size as packet
    }

    //tx already setup for zlp status if no data stage
    //rx for setup, unless out data stage, then rlen and data1
    ep->xfer(ep->TX, ep0txbuf, tlen, 1);
    ep->xfer(ep->RX, ep0rxbuf, rlen ? rlen : 64, rlen ? 1 : 0);

    return true;
}




//callback from ep0 trn in (tx zlp after set address setup packet)
//=============================================================================
bool set_address(UsbEP* ep)
//=============================================================================
{
    Usb::dev_addr(pkt.wValue);  //set usb address
    ep->set_notify(ep->TX, 0);  //clear notify (one time event)
    return true;
}











/*
UsbEP  ep0;
UsbEP  ep2;

void test(){
    ep0.init(0,64);
    ep2.init(2,64);
}

*/
