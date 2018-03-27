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
        m_buf[RX].addr = ep0buf;
        m_buf[RX].siz = 128;
        m_buf[RX].btogo = 64;
        m_ppbi[RX] = 0;
        m_ppbi[TX] = 0;
        setup(RX);
        release_tx();
    }
    Usb::epcontrol(n, UsbConfig::ep_ctrl[n]); //set endpoint control
    return true;
}

//=============================================================================
void UsbEP::set_callme_rx(callme_rx_t f)
//=============================================================================
{
    m_callme_rx = f;
}

//=============================================================================
void UsbEP::set_callme_tx(callme_tx_t f)
//=============================================================================
{
    m_callme_tx = f;
}

//=============================================================================
void UsbEP::set_other_req(other_req_t f)
//=============================================================================
{
    m_other_req = f;
}

//=============================================================================
bool UsbEP::setup(TXRX trx, bool stall)
//=============================================================================
{
debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\t%s  %08x %04x %d %s\r\n",trx?"TX":"RX",
        (uint32_t)m_buf[trx].addr,m_buf[trx].btogo,m_ppbi[trx],stall?"STALL":"");

    if(not m_buf[trx].addr) return false;   //no buffer set
    uint8_t i = (trx<<1) + m_ppbi[trx];     //bdt index
    if(m_bdt[i].stat.uown) i xor_eq 1;      //in use, try next
    if(m_bdt[i].stat.uown) return false;    //both in use

    volatile UsbBdt::ctrl_t ctrl = {0};
    ctrl.bstall = stall;
    ctrl.data01 = m_buf[trx].d01;
    ctrl.uown = 1;
    ctrl.dts = 1;
    ctrl.count = m_buf[trx].btogo < 64 ? m_buf[trx].btogo : 64;
    m_bdt[i].bufaddr = Reg::k2phys((uint32_t)m_buf[trx].addr);
    m_bdt[i].ctrl.val32 = ctrl.val32;

if(trx == TX) debug_bytes(m_buf[TX].addr, ctrl.count);
    return true;
}

//=============================================================================
void UsbEP::trn_service(uint8_t ustat) //called from ISR
//=============================================================================
{
    //ustat already shifted in isr, is 0-3 (rx/e, rx/o, tx/e, tx/o)
    m_idx = ustat;
    //update our copy of ppbi to opposite of ustat.ppbi (ustat & 1)
    m_ppbi[m_idx>>1] = not (m_idx bitand 1);
    //stat is pid, count, etc. from bdt table
    m_stat.val32 = m_bdt[m_idx].stat.val32;
    //set next data01
    m_buf[m_idx>>1].d01 = m_stat.data01 xor 1;

debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\tEP: %d\r\n", m_epnum);

    //if we are endpoint 0 and ep was rx, get next rx ready
    //if we are endpoint 0 and was setup pkt (rx), service setup pkt
    if(m_epnum == 0 and m_idx <= 1){
        //enable next RX
        buf_t& rx = m_buf[RX];
        rx.d01 = 0;
        //set next half of rx buffer to use 0-63 or 64-127
        rx.addr = rx.addr == &ep0buf[0] ? &ep0buf[64] : &ep0buf[0];
        rx.btogo = 64;
        rx.bdone = 0;
        setup(RX);
        if(m_stat.pid == UsbCh9::SETUP){
            setup_service();
            return;
        }
    }

    if(m_stat.pid == UsbCh9::OUT) out_service();
    else if(m_stat.pid == UsbCh9::IN) in_service();
}

//=============================================================================
void UsbEP::release_tx()
//=============================================================================
{
    if(m_buf[TX].addr){
        if(m_bdt[2].stat.uown){
            m_bdt[2].ctrl.uown = 0;
            m_ppbi[TX] xor_eq 1; //hardware ppbi advances when clearing
        }
        if(m_bdt[3].stat.uown){
            m_bdt[3].ctrl.uown = 0;
            m_ppbi[TX] xor_eq 1;
        }
    }
    UsbBuf::release(m_buf[TX].addr);
    m_buf[TX].addr = 0;
    m_buf[TX].siz = 0;
}

//=============================================================================
void UsbEP::setup_service()
//=============================================================================
{
    Usb usb; UsbBuf ubuf; UsbConfig ucfg;

    //get tx,rx buf_t
    buf_t& tx = m_buf[TX];
    buf_t& rx = m_buf[RX];

    //take back any pending tx on ep0
    release_tx();


    //TODO could double check if m_stat.count == 8

    //setup packet is in rx (8bytes)
    //since we already switched addr to other half of buffer
    uint8_t* a = rx.addr == &ep0buf[0] ? &ep0buf[64] : &ep0buf[0];
    UsbCh9::SetupPkt_t* pkt = (UsbCh9::SetupPkt_t*)a;

    //get our own tx buffers in setup_service

    //get a tx buffer
    if(pkt->wLength <= 64){
        tx.addr = ubuf.get64();
        tx.siz = 64;
    } else {
        tx.addr = ubuf.get512();
        tx.siz = 512;
    }
    if(not tx.addr){
        tx.siz = 0;
        return; //could not get a buffer (unlikely)
    }

    tx.addr[0] = 0;
    tx.addr[1] = 0;
    tx.d01 = 1;
    tx.zlp = false;
    tx.btogo = pkt->wLength;
    tx.bdone= 0;

    int16_t xlen = pkt->wLength; //total to xmit

debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\t%04x %04x %04x %04x\r\n",
       pkt->wRequest,pkt->wValue,pkt->wIndex,pkt->wLength);

    switch(pkt->wRequest){

        case UsbCh9::DEV_GET_STATUS:
            tx.addr[0] = ucfg.self_powered bitor (ucfg.remote_wakeup<<1);
            break;

        case UsbCh9::IF_GET_STATUS:
            xlen = -1;
            break;

        case UsbCh9::EP_GET_STATUS:
            tx.addr[0] = usb.epcontrol(pkt->wIndex, usb.EPSTALL);
            break;

        case UsbCh9::DEV_CLEAR_FEATURE:
            //remote wakup, test mode, ignore both
            //let status proceed
            break;

        case UsbCh9::IF_CLEAR_FEATURE:
            xlen = -1;
            break;

        case UsbCh9::EP_CLEAR_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //clear enpoint halt
            xlen = -1;
            break;

        case UsbCh9::DEV_SET_FEATURE:
            xlen = -1;
            break; //remote wakup, test mode, ignore both

        case UsbCh9::IF_SET_FEATURE:
            xlen = -1;
            break; //nothing

        case UsbCh9::EP_SET_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //set endpoint halt
            xlen = -1;
            break;

        case UsbCh9::DEV_SET_ADDRESS:
            break;
        case UsbCh9::DEV_GET_DESCRIPTOR:
            xlen = UsbDescriptors::get(pkt->wValue, tx.addr, pkt->wLength);
            if(xlen == 0) xlen = -1;
            break;

        case UsbCh9::DEV_SET_DESCRIPTOR:
            xlen = -1;
            break;

        case UsbCh9::DEV_GET_CONFIGURATION:
            xlen = UsbDescriptors::get(pkt->wValue, tx.addr, pkt->wLength );
            if(not xlen) xlen = -1;
            break;

        case UsbCh9::DEV_SET_CONFIGURATION:
            break;

        case UsbCh9::IF_GET_IFACE:
            xlen = -1;
            break;
        case UsbCh9::IF_SET_IFACE:
            xlen = -1;
            break;

        case UsbCh9::EP_SYNC_HFRAME:
            xlen = -1;
            break;

        default:
            xlen = -1;
            if(m_other_req) xlen = m_other_req(pkt, tx.addr, tx.siz);
            if(not xlen) xlen = -1;
            break;
    }

    //ignore whatever we don't support
    if(xlen == -1){
        //release_tx();
        tx.btogo = 0;
        setup(TX, true); //stall
        Usb::control(Usb::PKTDIS, false);
        return;
    }



    //if have less than requested, change xtogo
    //if then also falls on ep size boundary, also need a zlp to notify end
    if(xlen < pkt->wLength){
        tx.btogo = xlen;
        if((xlen % 64) == 0) tx.zlp = true;
    }
    setup(TX);

    //clear PKTDIS to let control xfer resume
    Usb::control(Usb::PKTDIS, false);
}

//=============================================================================
void UsbEP::out_service()
//=============================================================================
{
debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\tcount: %x\r\n",m_stat.count);
    //if count was 0 and was ep0 rx, was control zlp/status, done here
    if(m_stat.count == 0 and m_epnum == 0) return;
    //do something with other rx data here
    if(m_callme_rx) m_callme_rx(m_buf[RX].addr, m_stat.count);
}

//=============================================================================
void set_address()
//=============================================================================
{
    //check if need to set address
    if(Usb::dev_addr()) return;
    //not sure which one, check both for now
    //set_address is 0x500, if we find it, set address
    if(ep0buf[0] == 0 and ep0buf[1] == 5) Usb::dev_addr(ep0buf[2]);
    else if(ep0buf[64] == 0 and ep0buf[65] == 5) Usb::dev_addr(ep0buf[66]);

debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\tdev_addr: %02x\r\n",Usb::dev_addr());
}

//=============================================================================
void UsbEP::in_service()
//=============================================================================
{
    if(m_stat.count == 0 and m_epnum == 0){ //tx control status
        set_address();
    };

    buf_t& tx = m_buf[TX];

    tx.bdone += m_stat.count;               //adjust total xmitted
    if(m_stat.count > tx.btogo) tx.btogo = 0;//just in case, prevent uint wrap
    else tx.btogo -= m_stat.count;          //and amount left to do

debug("%s:%d:%s():\r\n", __FILE__, __LINE__, __func__);
debug("\tcount: %04x bdone: %04x  btogo: %04x\r\n",
       m_stat.count,tx.bdone,tx.btogo);

    if(not tx.btogo){                       //all done?
        if(tx.zlp){
            tx.zlp = false;
            setup(TX);                      //zlp
            return;
        }
        //if(m_epnum == 0) release_tx();      //release if ep0
        if(m_callme_tx) m_callme_tx();      //notify someone if needed
        release_tx();
        return;                             //done
    }
    //need more bytes xmitted
    //adjust buffer address up by amount already transmitted
    tx.addr = &tx.addr[tx.bdone];

    setup(TX);
}

//=============================================================================
bool UsbEP::xfer(TXRX trx, uint8_t* buf, uint16_t siz)
//=============================================================================
{
   buf_t& x = m_buf[trx];
   if(x.addr) return false; //is in use
   x.addr = buf;
   x.siz = siz;
   x.btogo = siz;
   x.bdone = 0;
   return setup(trx);
}
