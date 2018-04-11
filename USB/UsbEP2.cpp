#include "UsbEP2.hpp"
#include "UsbBdt.hpp"
#include "UsbCh9.hpp"
#include "Usb.hpp"
#include "UsbBuf.hpp"
#include "Reg.hpp"
#include "UsbDescriptors.hpp"

#include <cstdint>
#include <cstring> //memset, memcpy
#include <cstdio>

//this file only, for ep0
static uint8_t ep0rxbuf[64] = {0};
static UsbCh9::SetupPkt_t pkt = {0};
static uint8_t* ep0txbuf;
static bool control (UsbEP2*);
static bool set_address(UsbEP2*);
//


//=============================================================================
    void    UsbEP2::reset            (TXRX trx, bool saveppbi)
//=============================================================================
{
    info_t& x = m_ep[trx];
    bool ppbi = saveppbi ? x.ppbi : 0;
    x = {0};
    x.ppbi = ppbi;
    x.bdt = &UsbBdt::table[m_epnum<<2 bitor trx<<1];
}

//=============================================================================
    bool    UsbEP2::init             (uint8_t n, uint16_t rxsiz, uint16_t txsiz)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num) return false;
    m_ep[0] = {0};
    m_ep[1] = {0};
    m_epnum = n;
    rx->epsiz = rxsiz;
    tx->epsiz = txsiz;
    rx->bdt = &UsbBdt::table[n<<2];
    tx->bdt = &UsbBdt::table[(n<<2)+2];
    reset(TX);
    reset(RX);
    Usb::epcontrol(n, UsbConfig::ep_ctrl[n]); //set endpoint control
    if(n) return true;
    //ep0
    return xfer(RX, ep0rxbuf, rxsiz);
}

//=============================================================================
    bool    UsbEP2::set_buf          (TXRX trx, uint8_t* buf, uint16_t siz)
//=============================================================================
{
    m_ep[trx].buf = buf;
    m_ep[trx].siz = siz;
    return true;
}

//=============================================================================
    bool    UsbEP2::set_notify       (TXRX trx, notify_t f)
//=============================================================================
{
    m_ep[trx].notify = f;
    return true;
}

//specify d01, notify optional (default = 0)
//=============================================================================
bool UsbEP2::xfer(TXRX trx, uint8_t* buf, uint16_t siz, bool d01, notify_t f)
//=============================================================================
{
    m_ep[trx].d01 = d01;
    return xfer(trx, buf, siz, f);
}

//=============================================================================
bool UsbEP2::xfer(TXRX trx, uint8_t* buf, uint16_t siz, notify_t f)
//=============================================================================
{
    info_t& x = m_ep[trx];
    x.buf = buf;
    x.siz = siz;
    x.btogo = siz;
    x.bdone = 0;
    if(f) x.notify = f;
    return setup(trx);
}

//=============================================================================
    bool    UsbEP2::busy                 (TXRX trx)
//=============================================================================
{
    info_t& x = m_ep[trx];
    return x.bdt[0].stat.uown or x.bdt[1].stat.uown;
}

//=============================================================================
    bool    UsbEP2::setup                (TXRX trx, bool stall)
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
    ctrl.dts = 1;
    ctrl.uown = 1;

    ctrl.count = x.btogo < 64 ? x.btogo : 64;
    x.bdt[i].bufaddr = Reg::k2phys((uint32_t)x.buf);
    x.bdt[i].ctrl.val32 = ctrl.val32;

    return true;
}

//called from ISR with rx/tx,even/odd (0-3)
//=============================================================================
    bool    UsbEP2::service              (uint8_t ustat)
//=============================================================================
{
    m_ustat = ustat bitand 3;
    info_t& x = m_ep[ustat>>1]; //tx or rx
    x.stat.val32 = x.bdt[ustat bitand 1].stat.val32;        //get bdt stat
    x.ppbi xor_eq 1;            //toggle ppbi
    x.d01 xor_eq 1;             //toggle d01

printf("service(%d) %d %d %08x\r\n",ustat,m_epnum,x.stat.pid,x.stat.val32);

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
                memcpy(&pkt.packet[0], rx->buf, sizeof(pkt));
                control(this);
                //clear PKTDIS to let control xfer resume
                Usb::control(Usb::PKTDIS, false);
            }
            break;
        default:
printf("unknown x.stat.pid: %d\r\n",x.stat.pid);
            return false;
    }
    return true;
}

//=============================================================================
    bool    UsbEP2::takeback             (TXRX trx)
//=============================================================================
{
    info_t& x = m_ep[trx];
    if(x.bdt[0].stat.uown){ //check even
        x.bdt[0].ctrl.uown = 0;
        x.ppbi xor_eq 1;
    }
    if(x.bdt[1].stat.uown){ //check odd
        x.bdt[1].ctrl.uown = 0;
        x.ppbi xor_eq 1;
    }
    reset(trx, true); //reset, but save ppbi
    return true;
}
//=============================================================================
    void    UsbEP2::txzlp                ()
//=============================================================================
{
    tx->zlp = true;
}
//=============================================================================
    void    UsbEP2::txin                 ()
//=============================================================================
{
    info_t& tx = m_ep[TX];

    tx.bdone += tx.stat.count;
    if(tx.stat.count > tx.btogo){
        tx.btogo = 0;
    } else tx.btogo -= tx.stat.count;

    if(tx.btogo){
        tx.buf += tx.stat.count;
        setup(TX);
    } else if(tx.zlp){
        tx.zlp = 0;
        setup(TX);
    } else if(tx.notify){
        tx.notify(this);
        tx.notify = 0;
    }

    if(m_epnum == 0 and tx.btogo == 0){
        UsbBuf::release(ep0txbuf);
    }
}

//=============================================================================
    void    UsbEP2::rxout                ()
//=============================================================================
{
    info_t& rx = m_ep[RX];

    rx.bdone += rx.stat.count;

    if(not rx.stat.count or           //zlp
        rx.stat.count > rx.btogo or //rx more than planned
        rx.stat.count < rx.epsiz){  //short packet
        rx.btogo = 0;                 //no more to do
    }
    else rx.btogo -= rx.stat.count;

    if(rx.btogo){
        rx.buf += rx.stat.count;
        setup(RX);
    } else if(rx.notify){
        rx.notify(this);
        rx.notify = 0;
    }

    if(not rx.btogo and m_epnum == 0){
        xfer(RX, ep0rxbuf, 64, false);
    }
}



//this file only- for ep0 only
//=============================================================================
static bool control (UsbEP2* ep)
//=============================================================================
{
    UsbConfig ucfg; Usb usb;

    //setup packet is now in pkt
    //take back any pending tx on ep0
    ep->takeback(ep->TX);

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

    //tx (IN) length requested, or status
    uint16_t tlen = pkt.wLength;
    //rx (OUT) length requested, or status
    uint16_t rlen = 0;


printf("pkt: $1%04x %04x %04x %04x$7\r\n",pkt.wRequest,pkt.wValue,pkt.wIndex,pkt.wLength);

    //process std req
    //pkt.wRequest, wValue, wIndex, wLength

    switch(pkt.wRequest){

        case UsbCh9::DEV_GET_STATUS:
            //tx 2bytes (data1), status (rx zlp data1)
            ep0txbuf[0] = ucfg.self_powered bitor (ucfg.remote_wakeup<<1);
            break;
        case UsbCh9::IF_GET_STATUS:
            //tx 2bytes 0x00 0x00 (data1), status (rx zlp data1)
            //already setup
            break;
        case UsbCh9::EP_GET_STATUS:
            //tx 2bytes (data1), status (rx zlp data1)
            ep0txbuf[0] = usb.epcontrol(pkt.wIndex, usb.EPSTALL);
            break;

        case UsbCh9::DEV_CLEAR_FEATURE:
            //no data, status (tx zlp)
            //remote wakup, test mode, ignore both
            //let status proceed
            break;

        case UsbCh9::EP_CLEAR_FEATURE:
            //no data, status (tx zlp)
            // only option is ENDPOINT_HALT (0x00) (not for ep0)
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, false);
            }
            break;

        case UsbCh9::DEV_SET_FEATURE:
            //no data, status (tx zlp)
            //remote wakup, test mode, ignore both
            //let status proceed
            break;

        case UsbCh9::EP_SET_FEATURE:
            //no data, status (tx zlp)
            //only option is ENDPOINT_HALT (0x00) (not for ep0)
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, true);
            }
            break;

        case UsbCh9::DEV_SET_ADDRESS:
            //no data, status (tx zlp)
            //set address via tx callback
            ep->set_notify(ep->TX, set_address);
            break;

        case UsbCh9::DEV_GET_DESCRIPTOR:
            UsbBuf::release(ep0txbuf);
            ep0txbuf = UsbBuf::get512();
            //tx tlen bytes (data1), status (rx zlp data1)
            tlen = UsbDescriptors::get(pkt.wValue, ep0txbuf, tlen);
            break;

        case UsbCh9::DEV_SET_DESCRIPTOR:
            //no data, status (tx zlp)
            break;

        case UsbCh9::DEV_GET_CONFIGURATION:
            //tx tlen bytes (data1), status (rx zlp data1)
            //send 0=not configured, anything else=configured
            ep0txbuf[0] = 1; //TODO
            break;

        case UsbCh9::DEV_SET_CONFIGURATION:
            //no data, status (tx zlp)
            //let status proceed
            break;

        case UsbCh9::IF_GET_IFACE:
            break;
        case UsbCh9::IF_SET_IFACE:
            break;
        case UsbCh9::EP_SYNC_HFRAME:
            break;

        case UsbCh9::CDC_SET_LINE_CODING:
            //rx data1, status (tx zlp)
            break;
        case UsbCh9::CDC_GET_LINE_CODING:
            //tx data1, status (rx zlp)
            break;
        case UsbCh9::CDC_SET_CONTROL_LINE_STATE:
            //no data, status (tx zlp)
            break;

        default:
            //check other requests
            break;
    }

    //check if no data for request (bad string index, etc)
    if((tlen == 0) and (rlen == 0) and pkt.wLength){
        //TODO stall
    }
    //check for tx data that is less than requested (strings,descriptors,etc)
    if(tlen and (tlen != pkt.wLength)){
        if(not (tlen % 64)) ep->txzlp(); //add zlp if same size as packet
    }

    ep->xfer(ep->TX, ep0txbuf, tlen, true); //true = data1
    ep->xfer(ep->RX, ep0rxbuf, 64, pkt.wLength ? true : false);

    return true;
}




//callback from ep0 trn in (tx zlp after set address setup packet)
//=============================================================================
bool set_address(UsbEP2* ep)
//=============================================================================
{
    Usb::dev_addr(pkt.wValue);  //set usb address
    return true;
}


