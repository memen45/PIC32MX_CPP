#pragma once

#include "UsbEP.hpp"
#include <cstdint>

struct UsbCdcAcm {

            static auto
init        (bool) -> bool;

            static auto
send        (uint8_t*, uint16_t, UsbEP::notify_t = 0) -> bool;

            //const string w/no embedded 0's
            static auto
send        (const char*, UsbEP::notify_t = 0) -> bool;

            static auto
recv        (uint8_t*, uint16_t, UsbEP::notify_t = 0) -> bool;

            static auto
is_active   () -> bool;

            enum
TXRX        : bool
            { SEND, RECV };

            static auto
busy        (TXRX) -> bool;

};

/*

Ubuntu based systems with ModeManager running, can take up to 30 seconds
before it will let you have /dev/ttyACM0 (baud rate changes about 4 times,
dtr/rts is to various states close to a doxen times)

if you have no need for ModemManager (gsm modems, I presume), just disable-

systemctl stop ModemManager.service
systemctl disable ModemManager.service

now, your cdc device is ready to be used moments after plugging in

*/
