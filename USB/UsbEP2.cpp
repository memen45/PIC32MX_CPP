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

//private
//=============================================================================
    void debug(const char* fmt, ...)
//=============================================================================
{
    if(not UsbConfig::debug_on) return;
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
//=============================================================================
    void debug_bytes(uint8_t* buf, uint16_t count)
//=============================================================================
{
    if(not UsbConfig::debug_on) return;
    for(auto i = 0; i < count; i++) printf("%s%02x",i%16==0?"\r\n\t":" ",buf[i]);
    printf("\r\n");
}
//=============================================================================
    void debug_func(const char* f)
//=============================================================================
{
    if(not UsbConfig::debug_on) return;
    printf("%16s: ",f);
}

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
debug_func(__func__);
debug(" n: %d  rxsiz: %d  txsiz: %d\r\n",n,rxsiz,txsiz);

    if(n > UsbConfig::last_ep_num) return false;
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
debug_func(__func__);
debug(" %s  buf: %08x  btogo: %dx$7\r\n",trx?"$3TX":"$2RX",(uint32_t)x.buf,x.btogo);
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
    ctrl.uown = 1;
    ctrl.dts = 1;
    ctrl.count = x.btogo < 64 ? x.btogo : 64;
    x.bdt[i].bufaddr = Reg::k2phys((uint32_t)x.buf);
    x.bdt[i].ctrl.val32 = ctrl.val32;

debug_func(__func__);
debug(" ppbi: %d bufaddr: %08x  ctrl: %08x\r\n",i,x.bdt[i].bufaddr,x.bdt[i].ctrl.val32);
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

debug_func(__func__);
debug(" %s:%s",m_ustat<2?"$2RX":"$3TX",m_ustat&1?"ODD ":"EVEN");
debug("  EP: %d  pid: %02x  bdt-stat: %08x$7\r\n", m_epnum,x.stat.pid,x.stat.val32);

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
                //rx,tx setup by control()
            }
            break;
        default:
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
debug_func(__func__);
debug(" $3count: %d",tx->stat.count);
if(tx->notify) debug("  notify: %08x",(uint32_t)tx->notify);
debug("$7\r\n");

    tx->bdone += tx->stat.count;
    if(tx->stat.count > tx->btogo){
        tx->btogo = 0;
    } else tx->btogo -= tx->stat.count;

    if(tx->btogo){
        tx->buf += tx->stat.count;
        setup(TX);
    } else if(tx->zlp){
        tx->zlp = 0;
        setup(TX);
    } else if(tx->notify){
        tx->notify(this);
        tx->notify = 0;
    }

    if(m_epnum == 0 and tx->btogo == 0) UsbBuf::release(ep0txbuf);
}

//=============================================================================
    void    UsbEP2::rxout                ()
//=============================================================================
{
debug_func(__func__);
debug(" $2count: %d$7\r\n", rx->stat.count);
    rx->bdone += rx->stat.count;

    if(not rx->stat.count or           //zlp
        rx->stat.count > rx->btogo or //rx more than planned
        rx->stat.count < rx->epsiz){  //short packet
        rx->btogo = 0;                 //no more to do
    } else rx->btogo -= rx->stat.count;

    if(rx->btogo){
        rx->buf += rx->stat.count;
        setup(RX);
    } else if(rx->notify){
        rx->notify(this);
        rx->notify = 0;
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
    //in all cases after a setup, next tx is data1 for either data or status
    ep->tx->d01 = 1;
    //assume rx needs to be set for next setup (change if need rx dat phase)
    ep->rx->d01 = 0;

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


debug_func(__func__);
debug(" $1%04x %04x %04x %04x %s$7\r\n",
       pkt.wRequest,pkt.wValue,pkt.wIndex,pkt.wLength,Usb::epcontrol(0,Usb::EPSTALL)?"STALLED":"");

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
            UsbBuf::release(ep0txbuf);
            ep0txbuf = UsbBuf::get512();
            //setup (here), tx tlen bytes (data1), status (rx zlp data1)
            tlen = UsbDescriptors::get(pkt.wValue, ep0txbuf, pkt.wLength);
            if(not tlen) tlen = -1;
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
bool set_address(UsbEP2* ep)
//=============================================================================
{
debug_func(__func__);
debug(" dev_addr: %d\r\n",pkt.wValue);
    Usb::dev_addr(pkt.wValue);  //set usb address
    return true;
}

