#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "UsbBdt.hpp"

struct UsbEP {

    enum TXRX : bool { RX = 0, TX = 2 };
    enum EVEODD : bool { EVEN, ODD };

    bool init(uint8_t);
    void setbuf(uint8_t* buf, TXRX, trx, EVEODD e, uint16_t siz); //specify
    void setbuf(uint8_t* buf, TXRX, trx, uint16_t siz); //current
    uint8_t* getbuf(TXRX trx, EVEODD e); //specify even/odd
    uint8_t* getbuf(TXRX trx); //current one (not in use)
    bool setup(TXRX trx, EVEODD e, uint16_t count, uint8_t opt); //specify even/odd
    bool setup(TXRX trx, EVEODD e, UsbBdt::ctrl_t ctrl);
    bool setup(TXRX trx, uint16_t count, uint8_t opt); //current
    bool setup(TXRX trx, UsbBdt::ctrl_t ctrl);

    private:

    bool setup_service(uint8_t n, UsbBdt::stat_t stat);
    bool out_service(uint8_t n, UsbBdt::stat_t stat);
    bool in_service(uint8_t n, UsbBdt::stat_t stat);

    const uint8_t           m_epnum{0};
    const UsbBdt::bdt_t*    m_bdt{0};

    using buf_t = struct {
        bool                eveodd; //current unused buffer
        bool                active; //uown set (our bit)
        uint8_t*            addr;   //buffer address (virtual)
        uint16_t            siz;    //buffer size
        uint16_t            count;  //bdt set count (<= max packet size)
        uint16_t            xmitted;//total bytes xmitted
        uint16_t            xmitbytes; //total to xmit
    };
    buf_t                   m_buf[4]{0};

};





