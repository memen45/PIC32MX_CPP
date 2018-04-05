#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "UsbBdt.hpp"
#include "UsbCh9.hpp"

struct UsbEP {

    enum TXRX : bool { RX = 0, TX = 1 };

    using callme_t = int16_t(*)(uint8_t*, uint16_t, UsbCh9::SetupPkt_t*);

    bool        init            (uint8_t);
    void        set_callme_rx   (callme_t);
    void        set_callme_tx   (callme_t);
    void        set_other_req   (callme_t);
    bool        setup           (TXRX);
    void        trn_service     (uint8_t);
    bool        xfer            (TXRX, uint8_t*, uint16_t);

    private:

    void        setup_service   ();
    void        out_service     ();
    void        in_service      ();
    void        release_tx      ();

    uint8_t         m_epnum{0};
    volatile UsbBdt::bdt_t*  m_bdt{0};
    UsbBdt::stat_t  m_stat{0};  //bdt stat
    uint8_t         m_idx{0};   //usb hardware stat, 0-3, index into m_bdt
                                //and >>1 = index into m_buf (RX or TX)


    callme_t m_callme_rx{0};
    callme_t m_callme_tx{0};
    callme_t m_other_req{0};

    using buf_t = struct {
        uint8_t*    addr;       //buffer address
        uint16_t    epsiz;      //endpoint size from descriptors
        uint16_t    siz;        //buffer size
        uint16_t    bdone;      //total bytes done
        uint16_t    btogo;      //total bytes to go
        bool        zlp;        //need zero length end packet
        bool        d01;        //data01
        bool        ppbi;       //our ppbi from usb stat, so is lagging
        bool        stall;
    };
    buf_t       m_rx{0};
    buf_t       m_tx{0};

};


#if 0

ppbi uown
 0    0     even, cpu
 0    1     even, sie
 1    0     odd, cpu
 1    1     odd, sie

trn
m_ppbi[m_idx>>1] = not (m_idx bitand 1)


reset, init rx
m_ppbi[0] = 0
m_ppbi[1] = 0

trn out/rx (setup packet)
m_ppbi[0] = 1 //toggle ppbi
m_ppbi[1] = 0

send zlp
m_ppbi[0] = 0
m_ppbi[1] = 0

trn in
m_ppbi[0] = 0
m_ppbi[1] = 1 //toggle ppbi



reset, init rx
m_ppbi[0] = 0
m_ppbi[1] = 0

trn out/rx (setup packet)
m_ppbi[0] = 1 //toggle ppbi
m_ppbi[1] = 0

in
m_ppbi[0] = 0
m_ppbi[1] = 0

zlp
m_ppbi[0] = 0
m_ppbi[1] = 0

trn in
m_ppbi[0] = 0
m_ppbi[1] = 1 //toggle ppbi

#endif



