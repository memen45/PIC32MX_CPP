#include "UsbEP.hpp"
#include "UsbCh9.hpp"
#include "Usb.hpp"
#include "UsbBuf.hpp"
#include "Reg.hpp"
#include "UsbCentral.hpp"

#include <cstdint>
#include <cstring>  //memset, memcpy
#include <cstdio>   //debug printf


//=============================================================================
    void    UsbEP::reset            (TXRX tr, bool saveppbi)
//=============================================================================
{
    info_t& x = tr ? m_tx : m_rx;
    x.buf = 0;
    x.bdone = 0;
    x.btogo = 0;
    x.zlp = 0;
    x.d01 = 0;
    x.stall = 0;
    x.notify = 0;
    if(not saveppbi) x.ppbi = 0;
}

//=============================================================================
    void    UsbEP::init             (uint8_t n)
//=============================================================================
{
    m_epnum = n;
    reset(TX);
    reset(RX);
    m_rx.epsiz = UsbCentral::get_epsiz(m_epnum, 0);
    m_tx.epsiz = UsbCentral::get_epsiz(m_epnum, 1);
    m_rx.bdt = Usb::bdt_addr(m_epnum, 0, 0); //0=rx, 0=even
    m_tx.bdt = Usb::bdt_addr(m_epnum, 1, 0); //1=tx, 0=even
    Usb::epcontrol(n, UsbCentral::get_epctrl(n)); //set endpoint control
}

//=============================================================================
    bool    UsbEP::xfer     (info_t& x, uint8_t* buf, uint16_t siz, notify_t f)
//=============================================================================
{
    x.buf = buf;
    x.btogo = siz;
    x.bdone = 0;
    if(f) x.notify = f;
    return setup(x);
}

//specify d01, notify optional (default = 0)
//=============================================================================
    bool    UsbEP::send     (uint8_t* buf, uint16_t siz, bool d01, notify_t f)
//=============================================================================
{
    m_tx.d01 = d01;
    return xfer(m_tx, buf, siz, f);
}

//=============================================================================
    bool    UsbEP::send     (uint8_t* buf, uint16_t siz, notify_t f)
//=============================================================================
{
    return xfer(m_tx, buf, siz, f);
}

//specify d01, notify optional (default = 0)
//=============================================================================
    bool    UsbEP::recv     (uint8_t* buf, uint16_t siz, bool d01, notify_t f)
//=============================================================================
{
    m_rx.d01 = d01;
    return xfer(m_rx, buf, siz, f);
}

//=============================================================================
    bool    UsbEP::recv     (uint8_t* buf, uint16_t siz, notify_t f)
//=============================================================================
{
    return xfer(m_rx, buf, siz, f);
}

//private
//=============================================================================
    static bool    m_busy               (UsbEP::info_t& x)
//=============================================================================
{
    return x.bdt[UsbEP::EVEN].stat.uown or
        x.bdt[UsbEP::ODD].stat.uown or
        x.btogo or
        x.zlp;
}
//=============================================================================
    bool    UsbEP::send_busy            ()
//=============================================================================
{
    return m_busy(m_tx);
}

//=============================================================================
    bool    UsbEP::recv_busy            ()
//=============================================================================
{
    return m_busy(m_rx);
}

//=============================================================================
    void    UsbEP::send_stall           ()
//=============================================================================
{
    m_tx.stall = true;
}

//=============================================================================
    void    UsbEP::recv_stall           ()
//=============================================================================
{
    m_rx.stall = true;
}

//=============================================================================
    void    UsbEP::send_zlp             ()
//=============================================================================
{
    m_tx.zlp = true;
}

//=============================================================================
    void    UsbEP::send_notify          (notify_t f)
//=============================================================================
{
    if(f) m_tx.notify = f;
}

//=============================================================================
    void    UsbEP::recv_notify          (notify_t f)
//=============================================================================
{
    if(f) m_rx.notify = f;
}



//=============================================================================
    bool    UsbEP::setup                (info_t& x)
//=============================================================================
{
    if(not x.buf) return false;             //no buffer set

    bool i = x.ppbi;
    if(x.bdt[i].stat.uown) i xor_eq 1;      //in use, try next
    if(x.bdt[i].stat.uown) return false;    //both in use

printf("%s %s %d (%d)\r\n",&x == &m_tx ? "TX" : "RX",i ? "ODD" : "EVEN",x.btogo,x.epsiz);

    volatile Usb::ctrl_t ctrl = {0};
    ctrl.bstall = x.stall;
    x.stall = false;                        //is set, now clear flag
    ctrl.data01 = x.d01;
    ctrl.dts = 1;
    ctrl.uown = 1;

    ctrl.count = x.btogo < x.epsiz ? x.btogo : x.epsiz;
    x.bdt[i].bufaddr = Reg::k2phys((uint32_t)x.buf);
    x.bdt[i].ctrl.val32 = ctrl.val32;

    return true;
}


//=============================================================================
    bool    UsbEP::service              (uint8_t ustat)
//=============================================================================
{
    info_t& x = ustat bitand 2 ? m_tx : m_rx; //tx or rx
    x.stat.val32 = x.bdt[ustat bitand 1].stat.val32; //get bdt stat
    x.ppbi xor_eq 1;            //toggle ppbi
    x.d01 xor_eq 1;             //toggle d01

printf("UsbEP::service   ustat: %d  ep: %d  pid: %d  bdt: %08x\r\n",ustat,m_epnum,x.stat.pid,x.stat.val32);

    //in (tx), out (rx)
    if(x.stat.pid == UsbCh9::OUT)       service_out();
    else if(x.stat.pid == UsbCh9::IN)   service_in();
    else                                return false;

    return true;
}

//=============================================================================
    void    UsbEP::takeback             (TXRX tr)
//=============================================================================
{
    info_t& x = tr ? m_tx : m_rx;
    if(x.bdt[EVEN].stat.uown){ //check even
        x.bdt[EVEN].ctrl.uown = 0;
        x.ppbi xor_eq 1;
    }
    if(x.bdt[ODD].stat.uown){ //check odd
        x.bdt[ODD].ctrl.uown = 0;
        x.ppbi xor_eq 1;
    }
    reset(tr, true); //reset, but save ppbi
}

//=============================================================================
    void    UsbEP::service_in               ()
//=============================================================================
{
    m_tx.bdone += m_tx.stat.count;
    if(m_tx.stat.count > m_tx.btogo){
        m_tx.btogo = 0;
    } else m_tx.btogo -= m_tx.stat.count;

    if(m_tx.btogo){
        m_tx.buf += m_tx.stat.count;
        setup(m_tx);
    } else if(m_tx.zlp){
        m_tx.zlp = 0;
        setup(m_tx);
    } else if(m_tx.notify){
        m_tx.notify(this);
        m_tx.notify = 0;
    }
}

//=============================================================================
    void    UsbEP::service_out              ()
//=============================================================================
{
    m_rx.bdone += m_rx.stat.count;

    if(not m_rx.stat.count or           //zlp
        m_rx.stat.count > m_rx.btogo or //rx more than planned
        m_rx.stat.count < m_rx.epsiz){  //short packet
        m_rx.btogo = 0;                 //no more to do
    }
    else m_rx.btogo -= m_rx.stat.count;

    if(m_rx.btogo){
        m_rx.buf += m_rx.stat.count;
        setup(m_rx);
    } else if(m_rx.notify){
        m_rx.notify(this);
        m_rx.notify = 0;
    }
}



//UsbEP0

static uint8_t              ep0rxbuf[64] = {0};


//this file only- for ep0 only
//callback from ep0 trn in (tx zlp after set address setup packet)
//=============================================================================
bool set_address(UsbEP* ep)
//=============================================================================
{
    //callbacks are UsbEP, cast to UsbEP0 in this case
    UsbEP0* ep0 = (UsbEP0*)ep;
    Usb::dev_addr(ep0->setup_pkt.wValue);  //set usb address
printf("set_address: %d\r\n",ep0->setup_pkt.wValue);
    return true;
}


//this file only- for ep0 only
//=============================================================================
static bool control (UsbEP0* ep0)
//=============================================================================
{
    Usb usb;
    static uint8_t* ep0txbuf;

    //setup packet is now in pkt
    //take back any pending tx on ep0
    ep0->takeback(ep0->TX);

    //release any previous tx (should already be released, just make sure)
    UsbBuf::release(ep0txbuf);
    ep0txbuf = 0;

    //get a tx buffer
    ep0txbuf = UsbBuf::get64();
    if(not ep0txbuf){
        return false; //could not get a buffer (unlikely)
    }

    //preset first 2 bytes
    ep0txbuf[0] = 0;
    ep0txbuf[1] = 0;

    //data length of tx
    uint16_t tlen = 0;

    //if data stage, device->host (IN, TX)
    if((ep0->setup_pkt.bmRequestType bitand (1<<7)) and ep0->setup_pkt.wLength){
        tlen = ep0->setup_pkt.wLength;
    }

    bool stall = false;


printf("pkt: $1%04x %04x %04x %04x$7\r\n",
       ep0->setup_pkt.wRequest,ep0->setup_pkt.wValue,ep0->setup_pkt.wIndex,ep0->setup_pkt.wLength);

    //process std req
    //pkt.wRequest, wValue, wIndex, wLength

    switch(ep0->setup_pkt.wRequest){

        case UsbCh9::DEV_GET_STATUS: //tx 2bytes, rx status
            ep0txbuf[0] = UsbCentral::get_status();
            break;
        case UsbCh9::IF_GET_STATUS: //tx 2bytes, rx status
            //bytes already already setup
            break;
        case UsbCh9::EP_GET_STATUS: //tx 2bytes, rx status
            ep0txbuf[0] = usb.epcontrol(ep0->setup_pkt.wIndex, usb.EPSTALL);
            break;
        case UsbCh9::DEV_CLEAR_FEATURE: //no data, tx status
            //remote wakup, test mode, do nothing
            break;
        case UsbCh9::EP_CLEAR_FEATURE: //no data, tx status
            // only option is ENDPOINT_HALT (0x00) (not for ep0)
            if(ep0->setup_pkt.wIndex and not ep0->setup_pkt.wValue){
                usb.epcontrol(ep0->setup_pkt.wIndex, usb.EPSTALL, false);
                //data toggle resets to data0
            }
            break;
        case UsbCh9::DEV_SET_FEATURE: //no data, tx status
            //remote wakup, test mode, do nothing
            break;
        case UsbCh9::EP_SET_FEATURE: //no data, tx status
            //only option is ENDPOINT_HALT (0x00) (not for ep0)
            if(ep0->setup_pkt.wIndex and not ep0->setup_pkt.wValue){
                usb.epcontrol(ep0->setup_pkt.wIndex, usb.EPSTALL, true);
            }
            break;
        case UsbCh9::DEV_SET_ADDRESS: //no data, tx status
            ep0->send_notify(set_address); //set address after status
            break;
        case UsbCh9::DEV_GET_DESCRIPTOR: //tx tlen bytes, rx status
            UsbBuf::release(ep0txbuf); //buffer not needed
            ep0txbuf = (uint8_t*)UsbCentral::get_desc(ep0->setup_pkt.wValue, &tlen);
            //if error, tlen=0 and ep0txbuf=0
            if((tlen == 0) or (ep0txbuf == 0)) stall = true;
            break;
//         case UsbCh9::DEV_SET_DESCRIPTOR:
//             //no data, status (tx zlp)
//             break;
        case UsbCh9::DEV_GET_CONFIGURATION: //tx tlen bytes, rx status
            //send 0=not configured, anything else=configured
            ep0txbuf[0] = usb.dev_addr() ? 1 : 0; //show configured if address set
            break;
        case UsbCh9::DEV_SET_CONFIGURATION: //no data, tx status
            if(usb.dev_addr() and not ep0->setup_pkt.wValue) usb.dev_addr(0);
            else UsbCentral::set_config(ep0->setup_pkt.wValue);
            //clear endpoint halt, data toggle resets to data0
            break;
//         case UsbCh9::IF_GET_IFACE:
//             break;
//         case UsbCh9::IF_SET_IFACE:
//             break;
//         case UsbCh9::EP_SYNC_HFRAME:
//             break;
        default:
            UsbBuf::release(ep0txbuf); //buffer not needed
            //check other requests
            if(UsbCentral::service(0, ep0)){ //true=all handled, except
                ep0->recv(ep0rxbuf, 64, false); //we take care of next setup
                return true;
            }
            //bad request, stall
            stall = true;
            break;
    }

    if(stall){
        ep0->send_stall();
        ep0->recv_stall();
        ep0->recv(ep0rxbuf, 64, false);
        return false;
    }

    //check for tx data that is less than requested (strings,descriptors,etc)
    //if less and if is same size as ep packet size, add zlp
    if(tlen and (tlen != ep0->setup_pkt.wLength) and (not (tlen % 64))) ep0->send_zlp();

    //true = data1, false=data0
    //tx is always data1- either status, or send data in data stage
    ep0->send(ep0txbuf, tlen, true);
    //if data stage, next rx is always data1 either rx data or rx status
    if(ep0->setup_pkt.wLength) ep0->recv(ep0rxbuf, 64, true);
    //next setup packet (data0) in all cases
    //(rx data stage is always <64, so there is only one rx data packet)
    ep0->recv(ep0rxbuf, 64, false);

    return true;
}


//=============================================================================
    void    UsbEP0::init             ()
//=============================================================================
{
    UsbEP::init(0);
    recv(ep0rxbuf, 64, false);      //first setup packet recv
}

//=============================================================================
    bool    UsbEP0::service              (uint8_t ustat)
//=============================================================================
{
info_t& x = ustat bitand 2 ? m_tx : m_rx; //tx or rx

    //check for in/out first, if returned false, check for setup here
    //(need to do first, as UsbEP::service also takes care of toggle ppbi
    // and data01, also gets bdt stat)
    if(UsbEP::service(ustat)) return true;

printf("UsbEP0::service   ustat: %d  ep: %d  pid: %d  bdt: %08x\r\n",ustat,m_epnum,x.stat.pid,x.stat.val32);
    //ep0 setup packet
    if(x.stat.pid == UsbCh9::SETUP and x.stat.count == 8){
        //make copy of setup packet
        memcpy(&setup_pkt.packet[0], m_rx.buf, 8);
        control(this);
        //clear PKTDIS to let control xfer resume
        Usb::control(Usb::PKTDIS, false);
        return true;
    }

printf("EP0 unknown x.stat.pid: %d\r\n",x.stat.pid);
    return false;
}

//=============================================================================
    void    UsbEP0::service_in              ()
//=============================================================================
{
    UsbEP::service_in();
    //release ep0 tx buffer here
    if(m_tx.btogo == 0) UsbBuf::release(m_tx.buf);
}

