#include "UsbBdt.hpp"
#include "Reg.hpp"
#include "UsbBuf.hpp"
#include "UsbCh9.hpp"

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>

//UsbBdt

UsbBdt::bdt_t UsbBdt::table[] = {0};

//=============================================================================
    void        UsbBdt::init     ()
//=============================================================================
{
    table = {0};
}


//UsbEP

//=============================================================================
bool UsbEP::init(uint8_t n)
//=============================================================================
{
    m_epnum = n bitand 15;
    if(n < UsbBdt::bdt_table_siz/4) return false;
    m_bdt = &UsbBdt::table[n<<2];
    m_buf = {0};
    return true;
}

//ep0.setbuf(UsbBuf::get64(), RX, EVEN, 64);
//ep0.setbuf(UsbBuf::get64(), RX, ODD, 64);
//=============================================================================
void UsbEP::setbuf(uint8_t* buf, TXRX trx, EVEODD e, uint16_t siz)
//=============================================================================
{
    uint8_t n = trx + e;
    m_buf[n].addr = buf;
    m_buf[n].siz = siz;
}

//set current buffer address, size
//ep0.setbuf(UsbBuf::get64(), RX, 64);
//=============================================================================
void UsbEP::setbuf(uint8_t* buf, TXRX trx, uint16_t siz)
//=============================================================================
{
    uint8_t n = trx + m_buf[trx].eveodd;
    m_buf[n].addr = buf;
    m_buf[n].siz = siz;
}

//=============================================================================
uint8_t* UsbEP::getbuf(TXRX trx, EVEODD e)
//=============================================================================
{
    return m_buf[trx+e].addr;
}

//=============================================================================
uint8_t* UsbEP::getbuf(TXRX trx)
//=============================================================================
{
    return m_buf[trx+m_buf[trx].eveodd].addr;
}

//=============================================================================
bool UsbEP::setup(TXRX trx, uint16_t count, uint8_t opt)
//=============================================================================
{
    bool eo = m_buf[trx+m_buf[trx].eveodd];
    return setup(trx, eo, count, opt);
}

//=============================================================================
bool UsbEP::setup(TXRX trx, EVEODD eo, uint16_t count, uint8_t opt)
//=============================================================================
{
    UsbBdt::ctrl_t ctrl = {0};
    ctrl.count = count;
    ctrl.val8 = opt;
    return setup(trx, eo, ctrl);
}

//=============================================================================
bool UsbEP::setup(TXRX trx, EVEODD eo, UsbBdt::ctrl_t ctrl)
//=============================================================================
{
    uint8_t n = trx+eo;
    if(is_active(trx, eo)) return false; //already in use
    if(not m_buf[n].addr) return false; //no buffer set
    m_bdt[n]->bufaddr = Reg::k2phys((uint32_t)m_buf[n].addr);
    m_buf[n].count = ctrl.count;
    m_buf[n].active = true;
    m_buf[n].xmitted = 0;
    m_bdt[n]->ctrl = ctrl;
    if(not m_buf[n xor 1].active){//switch to next buffer if not already active
        m_buf[trx].eveodd xor_eq 1; //next available is now current
    }
    return true;
}

//=============================================================================
bool UsbEP::setup(TXRX trx, uint16_t count, uint8_t opt)
//=============================================================================
{
    bool eo = m_buf[trx+m_buf[trx].eveodd];
    return setup(trx, eo, count, opt);
}

//=============================================================================
bool UsbEP::setup(TXRX trx, UsbBdt::ctrl_t ctrl)
//=============================================================================
{
    bool eo = m_buf[trx+m_buf[trx].eveodd];
    return setup(trx, eo, ctrl);
}

//=============================================================================
bool UsbEP::is_active(TXRX trx, EVEODD eo)
//=============================================================================
{
    return m_buf[trx+eo].active;
}

//=============================================================================
bool trn_service(uint8_t ustat)
//=============================================================================
{
    UsbBdt bdt; UsbCh9 ch9;

    //ustat already shifted, is 0-3
    m_buf[ustat].active = false; //we now must have whichever buffer was used
    //stat is pid, count, etc. from bdt table
    bdt.stat_t stat = m_bdt[ustat].stat;

    //if we are endpoint 0 and ep was rx, get next rx ready
    //if we are endpoint 0 and was setup pkt (rx), service setup pkt
    if(m_epnum == 0 and ustat <= 1){
        //enables RX for current bdt buffer
        bdt.ctrl_t bdc = {0};
        bdc.count = 64;
        bdc.uown = 1;
        setup(RX, bdc);
        if(stat.pid == ch9.SETUP) setup_service(ustat, stat);
    }

    if(stat.pid == ch9.OUT) out_service(ustat, stat);
    else if(stat.pid == ch9.IN) in_service(ustat, stat);
}

//=============================================================================
bool setup_service(uint8_t n, UsbBdt::stat_t stat)
//=============================================================================
{
    UsbBdt bdt; Usb usb; UsbBUf ubuf;

    //TODO take back any tx on ep0

    //TODO could double check if stat.count == 8

    //setup packet is in rx (8bytes)
    UsbCh9::SetupPkt_t* pkt = (UsbCh9::SetupPkt_t*)m_buf[n].addr;

    //get current tx buffer
    uint8_t* txbuf = m_buf[trx+m_buf[trx].eveodd].addr;
    uint16_t txsiz = m_buf[trx+m_buf[trx].eveodd].siz;

    auto release_tx = [&](){
        ubuf.release(txbuf);
        setbuf(0, TX, 0);
    }

    //if not previoulsy set, get a buffer
    if(not txbuf){
        txbuf = ubuf.get64();
        if(not txbuf) return false; //can't get a tx buffer
        setbuf(txbuf, TX, 64);
        txsiz = 64;
    }
    if(txsiz < pkt->wLength){ //get bigger buffer
        release_tx();
        txbuf = ubuf.get512();
        if(not txbuf) return false;
        setbuf(txbuf, TX, 512);
        txsiz = 512;
    }
    txbuf[0] = 0;
    txbuf[1] = 0;

    bdt.ctrl bdc = {0};
    bdc.uown = 1;
    bdc.data01 = 1; //start with data1 (also if no data stage)
    bdc.count = pkt->wLength < 64 ? pkt->wLength : 64; //max 64
    m_buf[n].xmitbytes = pkt->wLength; //total to xmit

    switch(pkt->wRequest){

        case DEV_GET_STATUS:
            txbuf[0] = UsbConfig::self_powered bitor
                       UsbConfig::remote_wakeup<<1;
            break;
        case IF_GET_STATUS:
            //nothing, ignore
            release_tx();
            return false;
        case EP_GET_STATUS:
            txbuf[0] = usb.epcontrol(pkt->wIndex, usb.EPSTALL);
            break;

        case DEV_CLEAR_FEATURE:
            //remote wakup, test mode, ignore both
            //let status proceed
            break;
        case IF_CLEAR_FEATURE:
            //nothing, ignore
            release_tx();
            return false;;
        case EP_CLEAR_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //clear enpoint halt
            break;

        case DEV_SET_FEATURE:
            //remote wakup, test mode, ignore both
            break;
        case IF_SET_FEATURE:
            //nothing
            break;
        case EP_SET_FEATURE:
            // only option is ENDPOINT_HALT (0x00)
            //set endpoint halt
            break;

        case DEV_SET_ADDRESS:

        case DEV_GET_DESCRIPTOR:
            UsbDescriptors::get(pkt->wValue, txbuf, txsiz);
            break;
        case DEV_SET_DESCRIPTOR:
            release_tx();
            return false; //ignore

        case DEV_GET_CONFIGURATION:
            uint16_t dsiz = UsbDescriptors::get(pkt->wValue, txbuf, txsiz );
            if(dsiz < pkt->wLength) m_buf[n].xmitbytes = dsiz; //smaller than requested
            break;
        case DEV_SET_CONFIGURATION:

        case IF_GET_IFACE:
        case IF_SET_IFACE:

        case EP_SYNC_HFRAME:

        default:
    }

    setup(TX, bdc);

}

//=============================================================================
bool out_service(uint8_t n, UsbBdt::stat_t stat)
//=============================================================================
{
}

//=============================================================================
bool in_service(uint8_t n, UsbBdt::stat_t stat)
//=============================================================================
{
    UsbBdt bdt; UsbBuf ubuf;
    m_buf[n].xmitted += stat.count;
    int16_t rem = m_buf[n].xmitbytes - m_buf[n].xmitted;
    if(rem <= 0){
        ubuf.release(m_buf[n].addr);
        setbuf(0, TX, n bitand 1);
        return; //done
    }
    //need more bytes xmitted, switch
    bdt.ctrl bdc = {0};
    bdc.uown = 1;
    bdc.data01 = stat.data01 xor 1; //next
    bdc.count = rem < 64 ? rem : 64 ; //max 64
    //copy buffer info to next buffer (keeping all info)
    uint8_t nb = n xor 1;
    m_buf[nb] = m_buf[n];
    //adjust address up by amount already transmitted
    m_buf[nb].addr = &m_buf[nb].addr[m_buf[nb].xmitted];
    setup(TX, bdc);
}

