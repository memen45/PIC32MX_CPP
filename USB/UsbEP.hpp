#pragma once

#include "UsbBdt.hpp"

#include <cstdint>


struct UsbEP {

    using notify_t = bool(*)(UsbEP*);

    enum TXRX : bool { RX, TX };
    enum EVEODD : bool { EVEN, ODD };

    void reset      (TXRX, bool = false); //bool = save ppbi?
    bool init       (uint8_t, uint16_t, uint16_t);
    bool set_buf    (TXRX, uint8_t*, uint16_t);
    bool set_notify (TXRX, notify_t);
    bool service    (uint8_t); //rx/tx,from ISR (0-3)
    bool xfer       (TXRX, uint8_t*, uint16_t, notify_t = 0);
    bool xfer       (TXRX, uint8_t*, uint16_t, bool, notify_t = 0);
    bool busy       (TXRX);
    bool takeback   (TXRX);
    void txin       ();
    void rxout      ();

    private:

    bool setup      (TXRX);


    uint8_t     m_epnum{0};         //endpoint number

    using info_t = struct {
        uint8_t*        buf;        //buffer address
        uint16_t        siz;        //buffer size
        uint16_t        epsiz;      //endpoint size
        uint16_t        bdone;      //total bytes done
        uint16_t        btogo;      //total bytes to go
        bool            zlp;        //need zero length end packet
        bool            d01;        //data01
        bool            ppbi;       //our ppbi
        bool            stall;      //do a stall (cleared by setup())
        UsbBdt::stat_t  stat;       //last bdt stat
        volatile UsbBdt::bdt_t* bdt;//bdt table ptr
        notify_t        notify;     //callback
    };
    info_t m_ep[2]{{0},{0}};      //can use index 0/1 (TXRX)

    public:
    info_t* rx{&m_ep[RX]};  //or use these
    info_t* tx{&m_ep[TX]};

    private:
    uint8_t m_ustat{0};     //last usb stat (from irq)- 0-3
                            //(dir and ppbi only)

};

