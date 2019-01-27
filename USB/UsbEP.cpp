#include "UsbEP.hpp"
#include "UsbCh9.hpp"
#include "Usb.hpp"
#include "Reg.hpp"
#include "UsbCentral.hpp"
#include "UsbDebug.hpp"

#include <cstdint>
#include <cstring>  //memset, memcpy

// private, debug use
static const char* m_filename = "UsbEP.cpp";


//=============================================================================
            auto UsbEP::
reset       (TXRX tr, bool saveppbi) -> void
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
            auto UsbEP::
init        (uint8_t n) -> void
            {
            m_epnum = n;
            reset(TX);
            reset(RX);
            m_rx.epsiz = UsbCentral::get_epsiz(m_epnum, 0);
            m_tx.epsiz = UsbCentral::get_epsiz(m_epnum, 1);
            m_rx.bdt = Usb::bdt_addr(m_epnum, 0, 0); //0=rx, 0=even
            m_tx.bdt = Usb::bdt_addr(m_epnum, 1, 0); //1=tx, 0=even
            Usb::epcontrol(n, UsbCentral::get_epctrl(n)); //set ep control
            }

//=============================================================================
            auto UsbEP::
xfer        (info_t& x, uint8_t* buf, uint16_t siz, notify_t f) -> bool
            {
            x.buf = buf;
            x.btogo = siz;
            x.bdone = 0;
            if(f) x.notify = f;
            return setup(x);
            }

//specify d01, notify optional (default = 0)
//=============================================================================
            auto UsbEP::
send        (uint8_t* buf, uint16_t siz, bool d01, notify_t f) -> bool
            {
            m_tx.d01 = d01;
            return xfer(m_tx, buf, siz, f);
            }

//=============================================================================
            auto UsbEP::
send        (uint8_t* buf, uint16_t siz, notify_t f) -> bool
            {
            return xfer(m_tx, buf, siz, f);
            }

//specify d01, notify optional (default = 0)
//=============================================================================
            auto UsbEP::
recv        (uint8_t* buf, uint16_t siz, bool d01, notify_t f) -> bool
            {
            m_rx.d01 = d01;
            return xfer(m_rx, buf, siz, f);
            }

//=============================================================================
            auto UsbEP::
recv        (uint8_t* buf, uint16_t siz, notify_t f) -> bool
            {
            return xfer(m_rx, buf, siz, f);
            }

//-----------------------------------------------------------------------------
            auto UsbEP::
busy        (TXRX e) -> bool
            {
            UsbEP::info_t& x = e == TX ? m_tx : m_rx;
            return x.bdt[UsbEP::EVEN].stat.uown or
                x.bdt[UsbEP::ODD].stat.uown or
                x.btogo or
                x.zlp;
            }

//=============================================================================
            auto UsbEP::
stall       (TXRX tr) -> void
            {
            if( tr == RX ) m_rx.stall = true; else m_tx.stall = true;
            }

//=============================================================================
            auto UsbEP::
send_zlp    () -> void
            {
            m_tx.zlp = true;
            }

//=============================================================================
            auto UsbEP::
send_notify (notify_t f) -> void
            {
            if(f) m_tx.notify = f;
            }

//=============================================================================
            auto UsbEP::
recv_notify (notify_t f) -> void
            {
            if(f) m_rx.notify = f;
            }

//=============================================================================
            auto UsbEP::
setup       (info_t& x) -> bool
            {
            if(not x.buf) return false;             //no buffer set, can't do

            bool i = x.ppbi;                        //get our ppbi
            if(x.bdt[i].stat.uown) i xor_eq 1;      //already in use, try next
            if(x.bdt[i].stat.uown) return false;    //both in use, cannot do

            // * * * * DEBUG * * * *
            UsbDebug dbg;
            if( dbg.debug() ){
                dbg.debug( m_filename, __func__,
                    "%s (%s) %d bytes",
                    &x == &m_tx ? "send" : "recv",
                    i ? "odd" : "even",
                    x.btogo );
            }
            // * * * * DEBUG * * * *


            volatile Usb::ctrl_t ctrl = {0};
            ctrl.bstall = x.stall;
            x.stall = false;                        //is set, now clear flag
            ctrl.data01 = x.d01;
            ctrl.dts = 1;
            ctrl.uown = 1;

            ctrl.count = x.btogo < x.epsiz ? x.btogo : x.epsiz; //limit size
            x.bdt[i].bufaddr = Reg::k2phys((uint32_t)x.buf); //physical address
            x.bdt[i].ctrl.val32 = ctrl.val32;

            return true;
            }


//=============================================================================
            auto UsbEP::
service     (uint8_t ustat) -> bool
            {
            info_t& x = ustat bitand 2 ? m_tx : m_rx;           //tx or rx
            x.stat.val32 = x.bdt[ustat bitand 1].stat.val32;    //get bdt stat
            x.ppbi xor_eq 1;                                    //toggle ppbi
            x.d01 xor_eq 1;                                     //toggle d01

            // * * * * DEBUG * * * *
            UsbDebug dbg;
            if( dbg.debug() ){
                dbg.debug( m_filename, __func__,
                    "ustat: %d  ep: %d  pid: %d  bdt: %08x",
                    ustat,m_epnum,x.stat.pid,x.stat.val32 );
            }
            // * * * * DEBUG * * * *

            if(x.stat.pid == UsbCh9::OUT)       service_out();  //out (rx)
            else if(x.stat.pid == UsbCh9::IN)   service_in();   //in (tx)
            else                                return false;   //is ep0 setup

            return true;
            }

//=============================================================================
            auto UsbEP::
takeback    (TXRX tr) -> void
            {
            info_t& x = tr ? m_tx : m_rx;
            if(x.bdt[EVEN].stat.uown){          //check even
                x.bdt[EVEN].ctrl.uown = 0;      //take
                x.ppbi xor_eq 1;                //ppbi toggles when uown 1->0
            }
            if(x.bdt[ODD].stat.uown){           //check odd
                x.bdt[ODD].ctrl.uown = 0;
                x.ppbi xor_eq 1;
            }
            reset(tr, true);                    //reset, but save (our) ppbi
            }

//=============================================================================
            auto UsbEP::
service_in  () -> void
            {
            m_tx.bdone += m_tx.stat.count;      //add count to bdone
            if(m_tx.stat.count > m_tx.btogo){   //if more than expected
                m_tx.btogo = 0;                 //set btogo to 0
            } else m_tx.btogo -= m_tx.stat.count;//else btogo -= count

            if(m_tx.btogo){                     //if more to do
                m_tx.buf += m_tx.stat.count;    //adjust buffer address
                setup(m_tx);                    //and setup for next tx
            } else if(m_tx.zlp){                //no more, but zlp set
                m_tx.zlp = 0;                   //clear zlp flag
                setup(m_tx);                    //and setup for tx (btogo is 0)
            } else if(m_tx.notify){             //no more, check if notify set
                notify_t f = m_tx.notify;       //need to copy
                m_tx.notify = 0;                //then clear it
                f(this);                        //call notify callback
                //clear notify before called, as callback may setup new xfer
                //with another callback (else we will clear the new callback)
            }
            }

//=============================================================================
            auto UsbEP::
service_out () -> void
            {
            m_rx.bdone += m_rx.stat.count;

            if(not m_rx.stat.count or           //zlp
                m_rx.stat.count > m_rx.btogo or //rx more than planned
                m_rx.stat.count < m_rx.epsiz){  //short packet
                    m_rx.btogo = 0;             //no more to do
            } else {
                m_rx.btogo -= m_rx.stat.count;  //subtract count
            }

            if(m_rx.btogo){                     //if more to do
                m_rx.buf += m_rx.stat.count;    //adjust buffer address
                setup(m_rx);                    //and setup for next rx
            } else if(m_rx.notify){             //no more, so check notify
                notify_t f = m_rx.notify;       //need to copy
                m_rx.notify = 0;                //then clear it
                f(this);                        //call notify callback
                //clear notify before called, as callback may setup new xfer
                //with another callback (else we will clear the new callback)
            }
            }



//UsbEP0

//local var, endpoint 0 rx buffer
//-----------------------------------------------------------------private-----
static uint8_t ep0rxbuf[64] = {0};

//callback from ep0 trn in (tx zlp after set address setup packet)
//-----------------------------------------------------------------private-----
            static auto
set_address (UsbEP* ep) -> bool
            {
            //callbacks are UsbEP, cast to UsbEP0 in this case
            UsbEP0* ep0 = (UsbEP0*)ep;
            Usb::dev_addr(ep0->setup_pkt.wValue);  //set usb address

            // * * * * DEBUG * * * *
            UsbDebug dbg;
            if( dbg.debug() ){
                dbg.debug( m_filename, __func__,
                    "usb address: %d",
                    ep0->setup_pkt.wValue );
            }
            // * * * * DEBUG * * * *

            return true;
            }

//ep0 standard requests
//-----------------------------------------------------------------private-----
            static auto
control     (UsbEP0* ep0) -> bool
            {
            Usb usb;
            //setup packet is now in pkt
            //take back any pending tx on ep0
            ep0->takeback(ep0->TX);
            //tx, only need a couple bytes for our use
            //preset first 2 bytes to 0
            static uint8_t txbuf[2] = {0, 0};
            //pointer to either buf above,
            //or set to something else below (descriptor)
            uint8_t* ep0txbuf = txbuf;

            bool txin = ep0->setup_pkt.bmRequestType bitand (1<<7);

            //if data stage (wLength > 0), and device->host (IN, TX)
            //set tlen (tx) to wLength, else is 0 (tx status)
            uint16_t tlen = (txin and ep0->setup_pkt.wLength) ?
                            ep0->setup_pkt.wLength : 0;

            //stall flag
            bool stall = false;

            // * * * * DEBUG * * * *
            UsbDebug dbg;
            if( dbg.debug() ){
                dbg.debug( m_filename, __func__,
                    "pkt: $1%02x %02x %04x %04x %04x$7",
                    ep0->setup_pkt.bmRequestType, ep0->setup_pkt.bRequest,
                    ep0->setup_pkt.wValue,
                    ep0->setup_pkt.wIndex,
                    ep0->setup_pkt.wLength );
            }
            // * * * * DEBUG * * * *


            //process std req
            //pkt.wRequest, wValue, wIndex, wLength

            switch(ep0->setup_pkt.wRequest){

                case UsbCh9::DEV_GET_STATUS: //tx 2bytes, rx status
                    txbuf[0] = UsbCentral::get_status();
                    break;
                case UsbCh9::IF_GET_STATUS: //tx 2bytes, rx status
                    //bytes already already setup
                    break;
                case UsbCh9::EP_GET_STATUS: //tx 2bytes, rx status
                    txbuf[0] = usb.epcontrol(ep0->setup_pkt.wIndex,
                                                usb.EPSTALL);
                    break;
                case UsbCh9::DEV_CLEAR_FEATURE: //no data, tx status
                    //remote wakeup, test mode, do nothing
                    break;
                case UsbCh9::EP_CLEAR_FEATURE: //no data, tx status
                    // only option is ENDPOINT_HALT (0x00) (not for ep0)
                    if(ep0->setup_pkt.wIndex and not ep0->setup_pkt.wValue){
                        usb.epcontrol(ep0->setup_pkt.wIndex, usb.EPSTALL,
                                      false);
                        //data toggle resets to data0
                    }
                    break;
                case UsbCh9::DEV_SET_FEATURE: //no data, tx status
                    //remote wakeup, test mode, do nothing
                    break;
                case UsbCh9::EP_SET_FEATURE: //no data, tx status
                    //only option is ENDPOINT_HALT (0x00) (not for ep0)
                    if(ep0->setup_pkt.wIndex and not ep0->setup_pkt.wValue){
                        usb.epcontrol(ep0->setup_pkt.wIndex, usb.EPSTALL,
                                      true);
                    }
                    break;
                case UsbCh9::DEV_SET_ADDRESS: //no data, tx status
                    ep0->send_notify(set_address); //set address after status
                    break;
                case UsbCh9::DEV_GET_DESCRIPTOR: //tx tlen bytes, rx status
                    ep0txbuf = (uint8_t*)UsbCentral::get_desc(
                                    ep0->setup_pkt.wValue, &tlen);
                    //if error, tlen=0 and ep0txbuf=0
                    if((tlen == 0) or (ep0txbuf == 0)){
                        stall = true;
                        ep0txbuf = txbuf;
                    }
                    break;
                case UsbCh9::DEV_SET_DESCRIPTOR:
                    //no data, tx status
                    break;
                case UsbCh9::DEV_GET_CONFIGURATION: //tx tlen bytes, rx status
                    //send 0=not configured, anything else=configured
                    //show configured if address set
                    txbuf[0] = usb.dev_addr() ? 1 : 0;
                    break;
                case UsbCh9::DEV_SET_CONFIGURATION: //no data, tx status
                    if(usb.dev_addr() and not ep0->setup_pkt.wValue){
                        usb.dev_addr(0);
                    }
                    else UsbCentral::set_config(ep0->setup_pkt.wValue);
                    //clear endpoint halt, data toggle resets to data0
                    break;
                case UsbCh9::IF_GET_IFACE:
                case UsbCh9::IF_SET_IFACE:
                case UsbCh9::EP_SYNC_HFRAME:
                    stall = true;
                    break;
                default:
                    //check other requests
                    if(UsbCentral::service(0, ep0)){ //true=all handled, except
                        ep0->recv(ep0rxbuf, 64, false); //we do next setup
                        return true;
                    }
                    //bad request, stall
                    stall = true;
                    break;
            }

            //if IN, will stall the data packet
            //if OUT, will stall the handshake
            if(stall){
               ep0->stall( ep0->TX );
            }

            //check for tx data that is less than requested
            //(strings,descriptors,etc)
            //if less and if is same size as ep packet size, add zlp
            if(tlen and (tlen != ep0->setup_pkt.wLength) and (not(tlen % 64))){
                ep0->send_zlp();
            }
            //true = data1, false=data0
            //tx is always data1- either status, or send data in data stage
            ep0->send(ep0txbuf, tlen, true);
            //if data stage, next rx is always data1- rx data or rx status
            if(ep0->setup_pkt.wLength) ep0->recv(ep0rxbuf, 64, true);
            //next setup packet (data0) in all cases
            //(rx data stage is always <64, so there is only 1 rx data packet)
            ep0->recv(ep0rxbuf, 64, false);

            return true;
            }

//=============================================================================
            auto UsbEP0::
init        () -> void
            {
            UsbEP::init(0);                 //same as others
            recv(ep0rxbuf, 64, false);      //and setup for first packet recv
            }

//=============================================================================
            auto UsbEP0::
service     (uint8_t ustat) -> bool
            {
            //check for in/out first, if returned false, check for setup here
            //(need to do first, as UsbEP::service also takes care of toggle
            //ppbi and data01, and gets bdt stat)
            if(UsbEP::service(ustat)) return true;

            info_t& x = ustat bitand 2 ? m_tx : m_rx; //tx or rx

            // * * * * DEBUG * * * *
            UsbDebug dbg;
            if( dbg.debug() ){
                dbg.debug( m_filename, __func__,
                    "ustat: %d  ep: %d  pid: %d  bdt: %08x",
                    ustat, m_epnum, x.stat.pid, x.stat.val32 );
            }
            // * * * * DEBUG * * * *

            //ep0 setup packet
            if(x.stat.pid == UsbCh9::SETUP and x.stat.count == 8){
                memcpy(&setup_pkt.packet[0], m_rx.buf, 8);  //copy
                control(this);                              //process
                Usb::control(Usb::PKTDIS, false);           //resume xfer
                return true;
            }
            return false;
            }

