#pragma once

//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include "UsbConfig.hpp"

struct UsbBdt {

    //USB Buffer Descriptor Control Format
    using ctrl_t = union {
        struct {
        unsigned        :2;
        unsigned bstall :1;
        unsigned dts    :1;
        unsigned ninc   :1;
        unsigned keep   :1;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        };
        uint8_t val8;
    };
    //USB Buffer Descriptor Status Format
    using stat_t = union {
        struct {
        unsigned        :2;
        unsigned pid    :4;
        unsigned data01 :1;
        unsigned uown   :1;
        unsigned        :8;
        unsigned count  :10;
        unsigned        :6;
        };
        uint8_t val8;
    };

    using bdt_t = struct {
        union { ctrt_t ctrl; stat_t stat; };
        uint32_t bufaddr;
    };

    //ep0 rx even   ctrl|stat,addr 2words/8bytes table[0]
    //ep0 rx odd    ctrl|stat,addr 2words/8bytes table[1]
    //ep0 tx even   ctrl|stat,addr 2words/8bytes table[2]
    //ep0 tx odd    ctrl|stat,addr 2words/8bytes table[3]
    //...

    static const bdt_table_siz = (UsbConfig::last_ep_num+1)*4;
    static volatile bdt_t table[bdt_table_siz]
        __attribute__ ((aligned (512)));

    //clear table
    static void         init        ();

};

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





