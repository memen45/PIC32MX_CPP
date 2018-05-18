#pragma once

#include "Usb.hpp"
#include "UsbCh9.hpp"

#include <cstdint>


struct UsbEP {

            using
notify_t    = bool(*)(UsbEP*);

            enum
TXRX        : bool { RX, TX };

            enum
EVEODD      : bool { EVEN, ODD };

            auto
reset       (TXRX, bool = false) -> void; //bool = save ppbi?

            auto
init        (uint8_t) -> void;

            auto
service     (uint8_t) -> bool;

            auto
service_in  () -> void;

            auto
service_out () -> void;

            auto
send        (uint8_t*, uint16_t, notify_t = 0) -> bool;

            //specify data01
            auto
send        (uint8_t*, uint16_t, bool, notify_t = 0) -> bool;

            auto
recv        (uint8_t*, uint16_t, notify_t = 0) -> bool;

            //specify data01
            auto
recv        (uint8_t*, uint16_t, bool, notify_t = 0) -> bool;

            auto
send_busy   () -> bool;

            auto
recv_busy   () -> bool;

            auto
send_stall  () -> void;

            auto
recv_stall  () -> void;

            auto
send_zlp    () -> void;

            auto
send_notify (notify_t) -> void;

            auto
recv_notify (notify_t) -> void;

            auto
takeback    (TXRX) -> void;

            using
info_t      = struct {
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

            auto
setup       (info_t&) -> bool;

            auto
xfer        (info_t&, uint8_t*, uint16_t, notify_t) -> bool;


            protected:

            uint8_t m_epnum{0};             //endpoint number
            info_t  m_rx{0};
            info_t  m_tx{0};

};

//endpoint 0 specific
struct UsbEP0 : public UsbEP {

            auto
init        () -> void;

            auto
service     (uint8_t) -> bool;


            UsbCh9::SetupPkt_t setup_pkt;

};

