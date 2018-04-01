#include "UsbEP2.hpp"
#include "UsbBdt.hpp"
#include "UsbCh9.hpp"

#include <cstdint>
#include <cstring> //memset

//this file only
static uint8_t ep0rxbuf[64] = {0};
static uint8_t pkt[8] = {0};
static uint8_t* ep0txbuf;
static bool control (UsbEP*);
static bool set_address(UsbEP*);
//

//=============================================================================
    void    UsbEP::reset            ()
//=============================================================================
{
    m_rx = {0};
    m_tx = {0};
    m_rx.bdt = &UsbBdt::table[m_epnum<<2]; //rx even/odd
    m_tx.bdt = &UsbBdt::table[m_epnum<<2 bitor (1<<1)]; //tx even/odd
}

//=============================================================================
    bool    UsbEP::init             (uint8_t n)
//=============================================================================
{
    if(n > UsbConfig::last_ep_num+1) return false;
    m_epnum = n;
    reset();
    if(n) return true;
    //ep0
    memset(pkt, 0, sizeof(pkt));
    memset(ep0rxbuf, 0, sizeof(ep0rxbuf));
    set_buf(RX, ep0rxbuf, sizeof(ep0rxbuf));
    return xfer(RX, ep0rxbuf, sizeof(ep0rxbuf));
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

//called from ISR with even/odd
//=============================================================================
    bool    UsbEP::service              (bool eo)
//=============================================================================
{
    info_t& x = m_ep[eo];
    //get bdt stat
    x.stat = x.bdt.stat;
    //toggle ppbi
    x.ppbi xor_eq 1;
    //set d01 to next data01
    x.d01 xor_eq x.stat.data01;


    //in (tx), out (rx)
    if(x.stat.pid == UsbCh9::OUT){
        //TODO out
    }
    else if(x.stat.pid == UsbCh9::IN){
        //TODO in
    }
    else if(x.stat.pid == UsbCh9::SETUP and
            m_epnum == 0 and
            x.stat.count == 8){
        //make copy of setup packet
        for(auto i = 0; i < sizeof(pkt); i++) pkt[i] = m_rx.buf[i];
        control(this);
    }
    else return false;
    return true;
}

//=============================================================================
    bool    UsbEP::takeback             (TXRX trx)
//=============================================================================
{
    info_t& x = m_ep[trx];
    if(x.bdt[0].stat.uown){
        x.bdt[0].uown = 0;
        x.ppbi xor_eq 1;
    }
    if(x.bdt[1].stat.uown){
        x.bdt[1].uown = 0;
        x.ppbi xor_eq 1;
    }
    x.buf = 0;
    x.siz = 0;
    x.btogo = 0;
    x.done = 0;
    x.zlp = 0;
}
//=============================================================================
    void    UsbEP::zlp                  (TXRX trx)
//=============================================================================
{
    m_ep[trx].zlp = true;
}



//this file only
//=============================================================================
static bool control (UsbEP* ep)
//=============================================================================
{
    UsbConfig ucfg; Usb usb;

    //setup packet is now in pkt
    //take back any pending tx on ep0
    ep->takeback(ep->TX);

    //release any previous tx
    UsbBuf::release(ep0txbuf);

    //get a tx buffer
    uint16_t bufsiz;
    if(pkt.wLength <= 64){
        ep0txbuf = UsbBuf::get64();
        bufsiz = 64;
    } else {
        ep0txbuf = UsbBuf::get512();
        bufsiz = 512;
    }
    if(not ep0txbuf){
        return false; //could not get a buffer (unlikely)
    }

    //preset first 2 bytes
    ep0txbuf[0] = 0;
    ep0txbuf[1] = 0;

    //assume we send/recv length requested
    uint16_t dlen = pkt.wLength;

    //process std req
    //pkt.wRequest, wValue, wIndex, wLength

    switch(pkt.wRequest){

        case UsbCh9::DEV_GET_STATUS:
            //setup (here), tx 2bytes (data1), status (rx zlp data1)
            //dlen should be 2
            ep0txbuf[0] = ucfg.self_powered bitor (ucfg.remote_wakeup<<1);
            break;
        case UsbCh9::IF_GET_STATUS:
            //setup (here), tx 2bytes 0x00 0x00 (data1), status (rx zlp data1)
            //dlen should be 2
            //already setup
            break;
        case UsbCh9::EP_GET_STATUS:
            //setup (here), tx 2bytes (data1), status (rx zlp data1)
            //dlen should be 2
            ep0txbuf[0] = usb.epcontrol(pkt.wIndex, usb.EPSTALL);
            break;

        case UsbCh9::DEV_CLEAR_FEATURE:
            //setup (here), no data, status (tx zlp)
            //remote wakup, test mode, ignore both
            //let status proceed
            //dlen should be 0
            break;

        case UsbCh9::EP_CLEAR_FEATURE:
            //setup (here), no data, status (tx zlp)
            // only option is ENDPOINT_HALT (0x00) (not for ep0)
            //dlen should be 0
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, false);
            }
            break;

        case UsbCh9::DEV_SET_FEATURE:
            //setup (here), no data, status (tx zlp)
            //remote wakup, test mode, ignore both
            //let status proceed
            //dlen should be 0
            break;

        case UsbCh9::EP_SET_FEATURE:
            //setup (here), no data, status (tx zlp)
            //only option is ENDPOINT_HALT (0x00) (not for ep0)
            //dlen should be 0
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, true);
            }
            break;

        case UsbCh9::DEV_SET_ADDRESS:
            //setup (here), no data, status (tx zlp)
            //set address via tx callback
            //dlen should be 0
            ep->set_notify(ep->TX, set_address);
            break;

        case UsbCh9::DEV_GET_DESCRIPTOR:
            //setup (here), tx dlen bytes (data1), status (rx zlp data1)
            dlen = UsbDescriptors::get(pkt.wValue, ep0txbuf, pkt.wLength);
            break;

        case UsbCh9::DEV_SET_DESCRIPTOR:
            //setup (here), no data, status (tx zlp)
            //dlen should be 0
            break;

        case UsbCh9::DEV_GET_CONFIGURATION:
            //setup (here), tx dlen bytes (data1), status (rx zlp data1)
            dlen = UsbDescriptors::get(pkt.wValue, ep0txbuf, pkt.wLength);
            break;

        case UsbCh9::DEV_SET_CONFIGURATION:
            //setup (here), no data, status (tx zlp)
            //let status proceed
            //dlen should be 0
            break;

        case UsbCh9::IF_GET_IFACE:
            break;
        case UsbCh9::IF_SET_IFACE:
            break;
        case UsbCh9::EP_SYNC_HFRAME:
            break;

        case UsbCh9::CDC_SET_LINE_CODING:
            break;
        case UsbCh9::CDC_GET_LINE_CODING:
            break;
        case UsbCh9::CDC_SET_CONTROL_LINE_STATE:
            break;

        default:
            //check other requests
            break;
    }

    //check for no data from request (bad string index, etc)
    if((dlen == 0) and pkt.wLength){
        //stall request
    }
    //check for tx data that is less than requested (strings,descriptors,etc)
    else if(dlen != pkt.wLength){
        if(not (dlen % 64)) ep->zlp(ep->TX); //add zlp if same size as packet
    }

    //tx already setup for zlp status if no data stage
    //setup tx or rx
    if(pkt.wRequest bitand 0x0080){             //is data in
        ep->xfer(ep->TX, ep0txbuf, dlen, 1);    //xfer tx data IN
        ep->xfer(ep->RX, ep0rxbuf, 0, 1);       //rx zlp status
    } else {                                    //is data out or no data
        ep->xfer(ep->TX, ep0txbuf, 0, 1);       //tx zlp status
        ep->xfer(ep->RX, ep0rxbuf, dlen, 1);    //rx dlen
    }


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
    ep0.init(0);
    ep2.init(2);
}

*/
