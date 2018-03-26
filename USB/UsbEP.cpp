#include "UsbEP.hpp"
#include "Reg.hpp"
#include "UsbBuf.hpp"
#include "UsbCh9.hpp"
#include "UsbBdt.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>


//take care of ep0 rx buffer space here
//use half (64) for even, half for odd
static uint8_t ep0buf[128] = {0};


//=============================================================================
bool UsbEP::init(uint8_t n)
//=============================================================================
{
    UsbBdt bdt;
    m_epnum = n bitand 15;
    if(n >= bdt.bdt_table_siz/4) return false;
    m_bdt = &bdt.table[n<<2]; //start of bdt table for this ep
    if(n == 0){
        ep0buf = {0}; //clear ep0 rx buf
        m_buf[0].addr = ep0buf;
        m_buf[0].siz = 128;
    }
    return true;
}

//=============================================================================
void UsbEP::set_callme_rx(callme_rx_t f);
//=============================================================================
{
    m_callme_rx = f;
}

//=============================================================================
void UsbEP::set_callme_tx(callme_tx_t f);
//=============================================================================
{
    m_callme_tx = f;
}

//=============================================================================
int16_t UsbEP::set_other_req(other_req_t f);
//=============================================================================
{
    m_other_req = f;
}

//=============================================================================
bool UsbEP::setup(TXRX trx)
//=============================================================================
{
    bdt_t& b = m_buf[trx];
    if(not b.addr) return false;            //no buffer set
    uint8_t i = (trx<<1) + m_ppbi[trx];     //bdt index
    if(m_bdt[i].stat.uown) i xor 1;         //in use, try next
    if(m_bdt[i].stat.uown) return false;    //both in use
    UsbBdt::ctrl_t ctrl = {0};
    ctrl.data01 = b.d01;
    ctrl.uown = 1;
    ctrl.count = b.btogo < 64 ? b.btogo : 64;
    m_bdt[i].bufaddr = Reg::k2phys((uint32_t)b.addr);
    m_bdt[i].ctrl = ctrl;
    return true;
}

//=============================================================================
bool UsbEP::trn_service(uint8_t ustat) //called from ISR
//=============================================================================
{
    UsbBdt bdt; UsbCh9 ch9;

    //ustat already shifted in isr, is 0-3 (rx/e, rx/o, tx/e, tx/o)
    m_idx = ustat;
    //update our copy of ppbi to opposite of ustat.ppbi (ustat & 1)
    m_ppbi[m_idx>>1] = not (m_idx bitand 1);
    //stat is pid, count, etc. from bdt table
    m_stat = m_bdt[m_idx].stat;
    //set next data01
    m_buf[m_idx].d01 = m_stat.data01 xor 1;

    //if we are endpoint 0 and ep was rx, get next rx ready
    //if we are endpoint 0 and was setup pkt (rx), service setup pkt
    if(m_epnum == 0 and m_idx <= 1){
        //enable next RX
        m_buf[RX].d01 = 0;
        m_buf[RX].addr xor_eq 64; //0-63 or 64-127
        m_buf[RX].btogo = 64;
        m_buf[RX].bdone = 0;
        setup(RX);
        if(m_stat.pid == ch9.SETUP) setup_service();
    }

    if(m_stat.pid == ch9.OUT) out_service();
    else if(stat.pid == ch9.IN) in_service();
}

//=============================================================================
static void release_tx()
//=============================================================================
{
    UsbBUf ubuf;
    ubuf.release(m_buf[TX].addr);
    m_buf[TX].addr = 0;
    m_buf[TX].siz = 0;
}

//=============================================================================
void UsbEP::setup_service()
//=============================================================================
{
    UsbBdt bdt; Usb usb; UsbBUf ubuf; UsbCh9 ch9; UsbConfig ucfg;

    //get tx buf_t
    buf_t& tx = m_buf[TX];

    //take back any pending tx on ep0
    if(tx.addr){
        if(m_bdt[2].stat.uown){
            m_bdt[2].ctrl.uown = 0;
            m_ppbi[TX] xor_eq 1; //hardware ppbi advances when clearing
        }
        if(m_bdt[3].stat.uown){
            m_bdt[3].ctrl.uown = 0;
            m_ppbi[TX] xor_eq 1;
        }
        release_tx();
    }

    //TODO could double check if m_stat.count == 8

    //setup packet is in rx (8bytes)
    //xor 64 because we already switched addr to other half of buffer
    ch9.SetupPkt_t* pkt = (ch9.SetupPkt_t*)m_buf[m_idx>>1].addr xor 64;

    //get our own tx buffers in setup_service

    //release if we still have
    release_tx();
    if(pkt->wLength > 64){
        tx.addr = ubuf.get64();
        tx.siz = 64;
    } else {
        tx.addr = ubuf.get512();
        tx.siz = 512;
    }
    if(not tx.addr){
        tx.siz = 0;
        return false; //could not get a buffer (unlikely)
    }

    tx.addr[0] = 0;
    tx.addr[1] = 0;
    tx.d01 = 1;

    uint16_t xlen = pkt->wLength; //total to xmit

    switch(pkt->wRequest){

        case DEV_GET_STATUS:
            tx.addr[0] = ucfg.self_powered bitor (ucfg.remote_wakeup<<1);
            break;

        case IF_GET_STATUS:
            xlen = -1;
            break;

        case EP_GET_STATUS:
            tx.addr[0] = usb.epcontrol(pkt->wIndex, usb.EPSTALL);
            break;

        case DEV_CLEAR_FEATURE:
            //remote wakup, test mode, ignore both
            //let status proceed
            break;

        case IF_CLEAR_FEATURE:
            xlen = -1;
            break;

        case EP_CLEAR_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //clear enpoint halt
            break;

        case DEV_SET_FEATURE:
            break; //remote wakup, test mode, ignore both

        case IF_SET_FEATURE:
            break; //nothing

        case EP_SET_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //set endpoint halt
            break;

        case DEV_SET_ADDRESS:

        case DEV_GET_DESCRIPTOR:
            xlen = UsbDescriptors::get(pkt->wValue, tx.addr, tx.siz);
            break;

        case DEV_SET_DESCRIPTOR:
            xlen = -1;
            break;

        case DEV_GET_CONFIGURATION:
            xlen = UsbDescriptors::get(pkt->wValue, tx.addr, tx.siz );
            if(not xlen) xlen = -1;
            break;

        case DEV_SET_CONFIGURATION:

        case IF_GET_IFACE:
        case IF_SET_IFACE:

        case EP_SYNC_HFRAME:

        default:
            xlen = -1;
            if(other_req) xlen = other_req(pkt, tx.addr, tx.siz))
            break;
    }

    //ignore whatever we don't support
    if(xlen == -1){
        release_tx();
        return;
    }

    tx.zlp = false;
    tx.btogo = pkt->wLength;
    tx.bdone= 0;

    //if have less than requested, change xmitbytes
    //if then also falls on ep size boundary, also need a zlp to notify end
    if(xlen < pkt->wLength){
        tx.btogo = xlen;
        if((xlen % 64) == 0) tx.zlp = true;
    }
    setup(TX);

    //clear PKTDIS to let control xfer resume
    usb.control(usb.PKTDIS, false);
}

//=============================================================================
void UsbEP::out_service()
//=============================================================================
{
    //if count was 0 and was ep0 rx, was control zlp/status, done here
    if(m_stat.count == 0 and m_epnum == 0) return;
    //do something with other rx data here
    if(callme_rx) callme_rx(m_buf[RX].addr, m_stat.count);
}

//=============================================================================
void UsbEP::in_service()
//=============================================================================
{
    UsbBdt bdt; UsbBuf ubuf;

    buf_t& tx = m_buf[TX];

    tx.bdone += m_stat.count;               //adjust total xmitted
    if(m_stat.count > tx.btogo) tx.btogo = 0;//just in case, prevent uint wrap
    else tx.btogo -= m_stat.count;          //and amount left to do
    if(not tx.btogo){                       //all done?
        if(tx.zlp){
            tx.zlp = false;
            setup(TX);                      //zlp
            return;
        }
        if(m_epnum == 0) release_tx();      //release if ep0
        if(callme_tx) callme_tx();          //notify someone if needed
        return;                             //done
    }
    //need more bytes xmitted
    //adjust buffer address up by amount already transmitted
    tx.addr = &tx.addr[tx.bdone];

    setup(TX);
}

//=============================================================================
bool UsbEP::send(uint8_t* buf, uint16_t siz)
//=============================================================================
{
    buf_t& tx = m_buf[TX];
    if(tx.addr) return false; //tx is in use
    tx.addr = buf;
    tx.siz = siz;
    tx.btogo = siz;
    tx.bdone = 0;
    tx.d01 = 1;
    return setup(TX);
}
