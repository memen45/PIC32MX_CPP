#pragma once

#include "Usb.hpp"
#include "UsbCh9.hpp"

#include <cstdint>


struct UsbEP {

    using notify_t = bool(*)(UsbEP*);

    enum TXRX : bool { RX, TX };
    enum EVEODD : bool { EVEN, ODD };

    void    reset       (TXRX, bool = false); //bool = save ppbi?
    void    init        (uint8_t);

    bool    service     (uint8_t);
    void    service_in  ();
    void    service_out ();

    bool    send        (uint8_t*, uint16_t, notify_t = 0);
    bool    send        (uint8_t*, uint16_t, bool, notify_t = 0); //specify data01
    bool    recv        (uint8_t*, uint16_t, notify_t = 0);
    bool    recv        (uint8_t*, uint16_t, bool, notify_t = 0); //specify data01

    bool    send_busy   ();
    bool    recv_busy   ();

    void    send_stall  ();
    void    recv_stall  ();
    void    send_zlp    ();

    void    send_notify (notify_t);
    void    recv_notify (notify_t);

    void    takeback    (TXRX);


    using info_t = struct {
        uint8_t*        buf;        //buffer address
        uint16_t        epsiz;      //endpoint size
        uint16_t        bdone;      //total bytes done
        uint16_t        btogo;      //total bytes to go
        bool            zlp;        //need zero length end packet
        bool            d01;        //data01
        bool            ppbi;       //our ppbi
        bool            stall;      //do a stall (cleared by setup())
        Usb::stat_t     stat;       //last bdt stat
        volatile Usb::bdt_t* bdt;   //bdt table ptr
        notify_t        notify;     //callback
    };

    private:

    bool    setup       (info_t&);

    bool    xfer        (info_t&, uint8_t*, uint16_t, notify_t);

    protected:

    uint8_t     m_epnum{0};         //endpoint number

    info_t      m_rx{0};
    info_t      m_tx{0};

};

//endpoint 0 specific
struct UsbEP0 : public UsbEP {

    void    init        ();

    bool    service     (uint8_t);

    UsbCh9::SetupPkt_t setup_pkt;

};

