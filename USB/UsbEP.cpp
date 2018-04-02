#include "UsbEP.hpp"
#include "Reg.hpp"
#include "UsbBuf.hpp"
#include "UsbCh9.hpp"
#include "UsbBdt.hpp"
#include "Usb.hpp"
#include "UsbDescriptors.hpp"
#include "UsbConfig.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include <cstdio>


//take care of ep0 rx buffer space here
//use half (64) for even, half for odd
static uint8_t ep0buf[128] = {0};

//callback for set address
int16_t set_address(uint8_t* buf, uint16_t sz, UsbCh9::SetupPkt_t* = 0);

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

//usb needs to be on, as we write to usb registers
//=============================================================================
bool UsbEP::init(uint8_t n)
//=============================================================================
{
    UsbBdt bdt;
    m_epnum = n bitand 15;
    if(n >= bdt.bdt_table_siz/4) return false;
    m_bdt = &bdt.table[n<<2]; //start of bdt table for this ep
    if(n == 0){
        //clear ep0 rx buf
        for(auto& i : ep0buf) i = 0;
        m_rx.addr = ep0buf;
        m_rx.siz = 128;
        m_rx.btogo = 64;
        m_rx.ppbi = 0;
        m_tx.ppbi = 0;
        setup(RX);
        release_tx();
    }
    Usb::epcontrol(n, UsbConfig::ep_ctrl[n]); //set endpoint control
    return true;
}

//=============================================================================
void UsbEP::set_callme_rx(callme_t f)
//=============================================================================
{
    m_callme_rx = f;
}

//=============================================================================
void UsbEP::set_callme_tx(callme_t f)
//=============================================================================
{
    m_callme_tx = f;
}

//=============================================================================
void UsbEP::set_other_req(callme_t f)
//=============================================================================
{
    m_other_req = f;
}

//=============================================================================
bool UsbEP::setup(TXRX trx)
//=============================================================================
{
    buf_t& x = trx ? m_tx : m_rx;

debug("%s:%d:%s(): [%s]", __FILE__, __LINE__, __func__, trx?"TX":"RX");
debug("  %08x %04x ppbi: %d  data01: %d  %s\r\n",
        (uint32_t)x.addr,x.btogo,x.ppbi,x.d01,x.stall?"STALL":"");

    if(not x.addr) return false;         //no buffer set
    uint8_t i = (trx<<1) + x.ppbi;              //bdt index
    if(m_bdt[i].stat.uown) i xor_eq 1;      //in use, try next
    if(m_bdt[i].stat.uown) return false;    //both in use

    volatile UsbBdt::ctrl_t ctrl = {0};
    ctrl.bstall = x.stall;
    ctrl.data01 = x.d01;
    ctrl.uown = 1;
    ctrl.dts = 1;
    ctrl.count = x.btogo < 64 ? x.btogo : 64;
    m_bdt[i].bufaddr = Reg::k2phys((uint32_t)x.addr);
    m_bdt[i].ctrl.val32 = ctrl.val32;

if(trx) debug_bytes(x.addr, ctrl.count);
    return true;
}

//=============================================================================
void UsbEP::trn_service(uint8_t ustat) //called from ISR
//=============================================================================
{
    //ustat already shifted in isr, is 0-3 (rx/e, rx/o, tx/e, tx/o)
    m_idx = ustat;
    //stat is pid, count, etc. from bdt table
    m_stat.val32 = m_bdt[m_idx].stat.val32;
    //update our copy of ppbi to opposite of ustat.ppbi (ustat & 1)
    //and toggle data01
    if(m_idx <= 1){ //rx
        m_rx.ppbi xor_eq 1;
        m_rx.d01 = m_stat.data01 xor 1;
    } else { //tx
        m_tx.ppbi xor_eq 1;
        m_tx.d01 = m_stat.data01 xor 1;
    }

debug("%s:%d:%s(): [%s:%d]", __FILE__, __LINE__, __func__,m_idx<2?"RX":"TX",m_idx&1);
debug("  EP: %d  pid: %02x  ustat: %02x  bdt-stat: %08x\r\n", m_epnum,m_stat.pid,ustat,m_stat.val32);

    //if we are endpoint 0 and ep was rx, get next rx ready
    //if we are endpoint 0 and was setup pkt (rx), service setup pkt
    if(m_epnum == 0 and m_idx <= 1){
        //enable next RX
        //TODO -data01
        m_rx.d01 = 0;
        //set next half of rx buffer to use 0-63 or 64-127
        m_rx.addr = m_rx.addr == &ep0buf[0] ? &ep0buf[64] : &ep0buf[0];
        m_rx.btogo = 64;
        m_rx.bdone = 0;
        if(m_stat.pid == UsbCh9::SETUP and m_stat.count == 8){
            setup_service();
            //clear PKTDIS to let control xfer resume
            Usb::control(Usb::PKTDIS, false);
        }
        else if(m_stat.pid == UsbCh9::OUT) out_service();
        setup(RX);
        return;
    }

    if(m_stat.pid == UsbCh9::OUT) out_service();
    else if(m_stat.pid == UsbCh9::IN) in_service();
}

//=============================================================================
void UsbEP::release_tx()
//=============================================================================
{
    if(m_tx.addr){
        if(m_bdt[2].stat.uown){
            m_bdt[2].ctrl.uown = 0;
            m_tx.ppbi xor_eq 1; //hardware ppbi advances when clearing
        }
        if(m_bdt[3].stat.uown){
            m_bdt[3].ctrl.uown = 0;
            m_tx.ppbi xor_eq 1;
        }
    }
    UsbBuf::release(m_tx.addr);
    m_tx.addr = 0;
    m_tx.siz = 0;
}

//=============================================================================
void UsbEP::setup_service()
//=============================================================================
{
    Usb usb; UsbBuf ubuf; UsbConfig ucfg;

    //take back any pending tx on ep0
    release_tx();

    //setup packet is in rx (8bytes)
    //since we already switched addr to other half of buffer
    uint8_t* a = m_rx.addr == &ep0buf[0] ? &ep0buf[64] : &ep0buf[0];
    UsbCh9::SetupPkt_t* pkt = (UsbCh9::SetupPkt_t*)a;

    //get our own tx buffers in setup_service

    //get a tx buffer
    if(pkt->wLength <= 64){
        m_tx.addr = ubuf.get64();
        m_tx.siz = 64;
    } else {
        m_tx.addr = ubuf.get512();
        m_tx.siz = 512;
    }
    if(not m_tx.addr){
        m_tx.siz = 0;
        return; //could not get a buffer (unlikely)
    }

    m_tx.addr[0] = 0;
    m_tx.addr[1] = 0;
    m_tx.d01 = 1;
    m_tx.zlp = false;
    m_tx.btogo = pkt->wLength;
    m_tx.bdone= 0;
    m_tx.stall = 0;

    int16_t xlen = pkt->wLength; //total to xmit

debug("%s:%d:%s():", __FILE__, __LINE__, __func__);
debug("  %04x %04x %04x %04x %s\r\n",
       pkt->wRequest,pkt->wValue,pkt->wIndex,pkt->wLength,Usb::epcontrol(0,Usb::EPSTALL)?"STALLED":"");

    switch(pkt->wRequest){

        case UsbCh9::DEV_GET_STATUS:
            m_tx.addr[0] = ucfg.self_powered bitor (ucfg.remote_wakeup<<1);
            break;

        case UsbCh9::IF_GET_STATUS:
            xlen = -1;
            break;

        case UsbCh9::EP_GET_STATUS:
            m_tx.addr[0] = usb.epcontrol(pkt->wIndex, usb.EPSTALL);
            break;

        case UsbCh9::DEV_CLEAR_FEATURE:
            //remote wakup, test mode, ignore both
            //let status proceed
            break;

        case UsbCh9::IF_CLEAR_FEATURE:
            break;

        case UsbCh9::EP_CLEAR_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //clear endpoint halt
            xlen = -1;
            break;

        case UsbCh9::DEV_SET_FEATURE:
            xlen = -2;
            break; //remote wakup, test mode, ignore both

        case UsbCh9::IF_SET_FEATURE:
            xlen = -2;
            break; //nothing

        case UsbCh9::EP_SET_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //set endpoint halt
            xlen = -2;
            break;

        case UsbCh9::DEV_SET_ADDRESS:
            set_callme_tx(set_address);
            break;
        case UsbCh9::DEV_GET_DESCRIPTOR:
            xlen = UsbDescriptors::get(pkt->wValue, m_tx.addr, pkt->wLength);
            if(xlen == 0) xlen = -1;
            break;

        case UsbCh9::DEV_SET_DESCRIPTOR:
            xlen = -2;
            break;

        case UsbCh9::DEV_GET_CONFIGURATION:
            xlen = UsbDescriptors::get(pkt->wValue, m_tx.addr, pkt->wLength );
            if(not xlen) xlen = -1;
            break;

        case UsbCh9::DEV_SET_CONFIGURATION:
            //ignore
            break;

        case UsbCh9::IF_GET_IFACE:
            xlen = -1;
            break;
        case UsbCh9::IF_SET_IFACE:
            xlen = -2;
            break;

        case UsbCh9::EP_SYNC_HFRAME:
            xlen = -1;
            break;

        case UsbCh9::CDC_SET_LINE_CODING:
        case UsbCh9::CDC_GET_LINE_CODING:
        case UsbCh9::CDC_SET_CONTROL_LINE_STATE:

        default:
            xlen = -1;
            if(m_other_req) xlen = m_other_req(m_tx.addr, m_tx.siz, pkt);
            break;
    }

    //stall whatever we don't support
    if(xlen < 0){
        xlen = 0;
        m_tx.stall = 1;
        m_rx.stall = 1;
    }

    //if have less than requested, change xtogo
    //if then also falls on ep size boundary, also need a zlp to notify end
    if(xlen < pkt->wLength){
        m_tx.btogo = xlen;
        if((xlen % 64) == 0) m_tx.zlp = true;
    }
    setup(TX);
}

//=============================================================================
void UsbEP::out_service()
//=============================================================================
{
debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\tcount: %x\r\n",m_stat.count);
    if(m_epnum){ //not ep0
        //do something with other rx data here
        if(m_callme_rx) m_callme_rx(m_rx.addr, m_stat.count,0);
        return;
    }
    //is ep0
    //if count was 0, was control zlp/status, done here
    if(m_stat.count == 0) return;
    if(m_callme_rx) m_callme_rx(m_rx.addr, m_stat.count,0);

}

//a tx callback
//=============================================================================
int16_t set_address(uint8_t* buf, uint16_t sz, UsbCh9::SetupPkt_t* pkt)
//=============================================================================
{
/*    //address is in previous buffer
    uint8_t* buf = m_rx.addr == &ep0buf[0] ? &ep0buf[64] : &ep0buf[0];
    //set_address is 0x500, if we find it, set address
    if(buf[0] == 0 and buf[1] == 5)*/ //Usb::dev_addr(buf[2]);
    Usb::dev_addr(pkt->wValue);

debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\tdev_addr: %02x\r\n",Usb::dev_addr());
    return 0;
}

//=============================================================================
void UsbEP::in_service()
//=============================================================================
{
//     if(m_stat.count == 0 and m_epnum == 0 and not Usb::dev_addr()){
//         set_address();
//     };

    m_tx.bdone += m_stat.count;               //adjust total xmitted
    if(m_stat.count > m_tx.btogo) m_tx.btogo = 0;//just in case, prevent uint wrap
    else m_tx.btogo -= m_stat.count;          //and amount left to do

debug("%s:%d:%s():", __FILE__, __LINE__, __func__);
debug("  count: %04x bdone: %04x  btogo: %04x\r\n",
       m_stat.count,m_tx.bdone,m_tx.btogo);

    if(not m_tx.btogo){                       //all done?
        if(m_tx.zlp){
            m_tx.zlp = false;
            setup(TX);                      //zlp
            return;
        }
        //if(m_epnum == 0) release_tx();      //release if ep0
        //notify someone if needed
        if(m_callme_tx){
            UsbCh9::SetupPkt_t* pkt =
                    m_rx.addr==&ep0buf[0]?
                        (UsbCh9::SetupPkt_t*)&ep0buf[64]:
                        (UsbCh9::SetupPkt_t*)&ep0buf[0];
            if(not m_callme_tx(0,0,pkt)){
                set_callme_tx(0); //only needed once
            }
        }
        release_tx();
        return;                             //done
    }
    //need more bytes xmitted
    //adjust buffer address up by amount already transmitted
    m_tx.addr = &m_tx.addr[m_tx.bdone];

    setup(TX);
}

//=============================================================================
bool UsbEP::xfer(TXRX trx, uint8_t* buf, uint16_t siz)
//=============================================================================
{
   buf_t& x = trx ? m_tx : m_rx;
   if(x.addr) return false; //is in use
   x.addr = buf;
   x.siz = siz;
   x.btogo = siz;
   x.bdone = 0;
   return setup(trx);
}
