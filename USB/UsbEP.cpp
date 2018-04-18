#include "UsbEP.hpp"
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
static bool control (UsbEP*);
static bool set_address(UsbEP*);

//line coding- just store what pc wants, also return if wanted
using line_coding_t = struct {
    uint32_t baud;
    uint8_t stop_bits; //0-1,1=1.5,2=2
    uint8_t parity; //none=0,even, odd, mark, space
    uint8_t data_bits; //5,6,7,8,16
};
static line_coding_t line_coding = {115200, 0, 0, 8};
static bool set_line_coding(UsbEP*);
//


void print_bytes(uint8_t* buf, uint16_t siz){
    for( ; siz; siz--){
        printf("%02x ",*buf++);
    }
    printf("\r\n");
}

//=============================================================================
    void    UsbEP::reset            (TXRX tr, bool saveppbi)
//=============================================================================
{
    info_t& x = m_ep[tr];
    bool ppbi = saveppbi ? x.ppbi : 0;
    x = {0};
    x.ppbi = ppbi;
    x.bdt = Usb::bdt_addr(m_epnum, tr, 0);//&Usb::bdt_table[m_epnum][tr][EVEN];
}

//=============================================================================
    bool    UsbEP::init             (uint8_t n)
//=============================================================================
{
    if(n > UsbDescriptors::last_ep_num) return false;
    m_epnum = n;
    reset(TX);
    reset(RX);
    rx->epsiz = UsbDescriptors::get_epsiz(n, 0);
    tx->epsiz = UsbDescriptors::get_epsiz(n, 1);
    Usb::epcontrol(n, UsbDescriptors::get_epctrl(n)); //set endpoint control
    if(n) return true;
    //ep0
    return xfer(RX, ep0rxbuf, rx->epsiz);
}

//=============================================================================
    bool    UsbEP::set_buf          (TXRX tr, uint8_t* buf, uint16_t siz)
//=============================================================================
{
    m_ep[tr].buf = buf;
    m_ep[tr].siz = siz;
    return true;
}

//=============================================================================
    bool    UsbEP::set_notify       (TXRX tr, notify_t f)
//=============================================================================
{
    m_ep[tr].notify = f;
    return true;
}

//specify d01, notify optional (default = 0)
//=============================================================================
bool UsbEP::xfer(TXRX tr, uint8_t* buf, uint16_t siz, bool d01, notify_t f)
//=============================================================================
{
    m_ep[tr].d01 = d01;
    return xfer(tr, buf, siz, f);
}

//=============================================================================
bool UsbEP::xfer(TXRX tr, uint8_t* buf, uint16_t siz, notify_t f)
//=============================================================================
{
    info_t& x = m_ep[tr];
    x.buf = buf;
    x.siz = siz;
    x.btogo = siz;
    x.bdone = 0;
    if(f) x.notify = f;
    return setup(tr);
}

//=============================================================================
    bool    UsbEP::busy                 (TXRX tr)
//=============================================================================
{
    info_t& x = m_ep[tr];
    return x.bdt[EVEN].stat.uown or x.bdt[ODD].stat.uown;
}

//=============================================================================
    bool    UsbEP::setup                (TXRX tr)
//=============================================================================
{
    info_t& x = m_ep[tr];
    if(not x.buf) return false;//no buffer set
    bool i = x.ppbi;
    if(x.bdt[i].stat.uown) i xor_eq 1;      //in use, try next
    if(x.bdt[i].stat.uown) return false;    //both in use

    volatile Usb::ctrl_t ctrl = {0};
    ctrl.bstall = x.stall; x.stall = false;
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
    bool    UsbEP::service              (uint8_t ustat)
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
    bool    UsbEP::takeback             (TXRX tr)
//=============================================================================
{
    info_t& x = m_ep[tr];
    if(x.bdt[EVEN].stat.uown){ //check even
        x.bdt[EVEN].ctrl.uown = 0;
        x.ppbi xor_eq 1;
    }
    if(x.bdt[ODD].stat.uown){ //check odd
        x.bdt[ODD].ctrl.uown = 0;
        x.ppbi xor_eq 1;
    }
    reset(tr, true); //reset, but save ppbi
    return true;
}

//=============================================================================
    void    UsbEP::txin                 ()
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
    void    UsbEP::rxout                ()
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
printf("rx notify\r\n");
        rx.notify(this);
        rx.notify = 0;
    }

}



//this file only- for ep0 only
//=============================================================================
static bool control (UsbEP* ep)
//=============================================================================
{
    Usb usb;

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

printf("pkt: $1%04x %04x %04x %04x$7\r\n",pkt.wRequest,pkt.wValue,pkt.wIndex,pkt.wLength);

    //process std req
    //pkt.wRequest, wValue, wIndex, wLength

    switch(pkt.wRequest){

        case UsbCh9::DEV_GET_STATUS: //tx 2bytes, rx status
            ep0txbuf[0] = UsbDescriptors::get_status();
            break;
        case UsbCh9::IF_GET_STATUS: //tx 2bytes, rx status
            //bytes already already setup
            break;
        case UsbCh9::EP_GET_STATUS: //tx 2bytes, rx status
            ep0txbuf[0] = usb.epcontrol(pkt.wIndex, usb.EPSTALL);
            break;
        case UsbCh9::DEV_CLEAR_FEATURE: //no data, tx status
            //remote wakup, test mode, do nothing
            break;
        case UsbCh9::EP_CLEAR_FEATURE: //no data, tx status
            // only option is ENDPOINT_HALT (0x00) (not for ep0)
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, false);
                //data toggle resets to data0
            }
            break;
        case UsbCh9::DEV_SET_FEATURE: //no data, tx status
            //remote wakup, test mode, do nothing
            break;
        case UsbCh9::EP_SET_FEATURE: //no data, tx status
            //only option is ENDPOINT_HALT (0x00) (not for ep0)
            if(pkt.wIndex and not pkt.wValue){
                usb.epcontrol(pkt.wIndex, usb.EPSTALL, true);
            }
            break;
        case UsbCh9::DEV_SET_ADDRESS: //no data, tx status
            ep->set_notify(ep->TX, set_address); //set address after status
            break;
        case UsbCh9::DEV_GET_DESCRIPTOR: //tx tlen bytes, rx status
            UsbBuf::release(ep0txbuf); //buffer not needed
            ep0txbuf = UsbDescriptors::get(pkt.wValue, &tlen);
            //if error, tlen=0 and ep0txbuf=0
            break;
//         case UsbCh9::DEV_SET_DESCRIPTOR:
//             //no data, status (tx zlp)
//             break;
        case UsbCh9::DEV_GET_CONFIGURATION: //tx tlen bytes, rx status
            //send 0=not configured, anything else=configured
            ep0txbuf[0] = usb.dev_addr() ? 1 : 0; //show configured if address set
            break;
        case UsbCh9::DEV_SET_CONFIGURATION: //no data, tx status
            if(usb.dev_addr() and not pkt.wValue) usb.dev_addr(0);
            else UsbDescriptors::set_config(pkt.wValue);
            //clear endpoint halt, data toggle resets to data0
            break;
//         case UsbCh9::IF_GET_IFACE:
//             break;
//         case UsbCh9::IF_SET_IFACE:
//             break;
//         case UsbCh9::EP_SYNC_HFRAME:
//             break;
        case UsbCh9::CDC_SET_LINE_CODING: //rx tlen bytes, tx status
            ep->set_notify(ep->RX, set_line_coding); //just to print it
            ep->xfer(ep->RX, (uint8_t*)&line_coding, 7, true);
            ep->xfer(ep->RX, ep0rxbuf, 64, false);
            ep->xfer(ep->TX, ep0txbuf, 0, true);
            return true;
            break;
        case UsbCh9::CDC_GET_LINE_CODING: //tx line coding, rx status
            memcpy(ep0txbuf, (void*)&line_coding, 7);
            break;
        case UsbCh9::CDC_SET_CONTROL_LINE_STATE: //no data, tx status
            break;
        default:
            //check other requests somwhere else
            //stall bad request
            ep->tx->stall = 1;
            ep->rx->stall = 1;
            break;
    }

    //check if no data for request (bad string index, etc)
    if((tlen == 0) and pkt.wLength){
        ep->tx->stall = 1;
        ep->rx->stall = 1;
    }
    //check for tx data that is less than requested (strings,descriptors,etc)
    //if less and if is same size as ep packet size, add zlp
    if(tlen and (tlen != pkt.wLength)){
        if(not (tlen % 64)) ep->tx->zlp = true;
    }

    //true = data1, false=data0
    //tx is always data1- either status, or send data in data stage
    ep->xfer(ep->TX, ep0txbuf, tlen, true);
    //if data stage, next rx is always data10 either rx data or rx status
    if(pkt.wLength) ep->xfer(ep->RX, ep0rxbuf, 64, true);
    //next setup packet (data0) in all cases
    //(rx data stage is always <64, so there is only on rx data packet)
    ep->xfer(ep->RX, ep0rxbuf, 64, false);

    return true;
}



//callback from ep0
//nothing to do, print just to see what is happening
//=============================================================================
bool set_line_coding(UsbEP* ep)
//=============================================================================
{
printf("%d %d %d %d\r\n",
    line_coding.baud,line_coding.stop_bits,line_coding.parity,
    line_coding.data_bits
);
   return true;
}

//callback from ep0 trn in (tx zlp after set address setup packet)
//=============================================================================
bool set_address(UsbEP* ep)
//=============================================================================
{
    Usb::dev_addr(pkt.wValue);  //set usb address
    return true;
}


