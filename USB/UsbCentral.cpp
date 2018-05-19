#include "UsbCentral.hpp"
#include "Usb.hpp"
#include "Irq.hpp"
#include "UsbEP.hpp"
#include "Delay.hpp"


#include <cstdio> //debug printf

//-----------------------------------------------------------------private-----
static UsbEP0                   ep0;
//specific device info
static const uint8_t*           m_descriptor = 0;
static UsbCentral::service_t    m_service = 0;
//counters
static uint32_t                 m_timer1ms = 0;
static uint32_t                 m_sofcount = 0;

//class vars
//=============================================================================
uint8_t UsbCentral::current_config = 1; //config 1


//-----------------------------------------------------------------private-----
            static auto
detach      () -> void
            {
            Usb usb; Irq irq;

            irq.on(irq.USB, false);             //usb irq off
            usb.reset();                        //usb regs to reset state

            //reset all endpoints in case was in the middle of something
            ep0.reset(ep0.TX);
            ep0.reset(ep0.RX);
            UsbCentral::service(0xFF);          //0xFF=reset
            }

//-----------------------------------------------------------------private-----
            static auto
attach      () -> void
            {
            Usb usb; Irq irq;

            usb.power(usb.USBPWR, true);        //power on (inits bdt table)
            ep0.init();                         //endpoint 0 init here
            UsbCentral::service(0xFF);          //others, 0xFF=reset endpoint
            usb.control(usb.USBEN, true);       //enable usb module
            usb.irqs(usb.SOF|usb.T1MSEC|usb.TRN|usb.IDLE); //start irq's
            irq.init(irq.USB, Usb::usb_irq_pri, Usb::usb_irq_subpri, true);
            irq.global(true);                   //global irq's on
            }

// init usb (or reinit, or detach) true=init/reinit, false=detach
// return true if attached, false if detached
// global irq's enabled if attached, unchanged if detached
//=============================================================================
            auto UsbCentral::
init        (bool tf) -> bool
            {
            //DEBUG
            printf("\r\n\r\nUsbCentral::init(%d)\r\n",tf);

            bool wason = Usb::power(Usb::USBPWR);
            detach();
            if(wason) Delay::wait_ms(200);      //if was already on, wait
            //if no vbus pin voltage or tf=false (wanted only detach)
            if(not Usb::vbus_ison() or not tf) return false;
            m_timer1ms = 0;                     //reset 1ms timer
            m_sofcount = 0;                     //and sof count
            attach();
            return true;                        //true=attached
            }

// called from UsbISR with irq flags which were set (and had irq enabled)
//=============================================================================
            auto UsbCentral::
service     (uint32_t flags, uint8_t ustat) -> void
            {
            Usb usb;

            if(flags bitand usb.TRN){
                //enable reset irq when any trn flags start arriving
                //so we only worry about reset irq when needed
                usb.irq(usb.URST, true);

                //DEBUG
                printf("\r\nUsbCentral::service  frame: %d  ustat: %d\r\n",
                       usb.frame(),ustat);

                if(ustat < 4){                  //endpoint 0 (ustat 0-3)
                    if(not ep0.service(ustat))  //if std request not handled
                        UsbCentral::service(ustat, &ep0); //let others try
                }
                else UsbCentral::service(ustat);//not endpoint 0
            }

            if(flags bitand usb.T1MSEC){
                m_timer1ms++;
                //check if physically detached (every ms), if so detach
                //no auto reattach, so app will need to take care of reattach
                if(not Usb::vbus_ison()) init(false);
            }
            if(flags bitand usb.SOF)        m_sofcount++;
            if(flags bitand usb.RESUME)     usb.irq(usb.RESUME, false);
            if(flags bitand usb.IDLE)       usb.irq(usb.RESUME, true);
            if(flags bitand usb.URST)       init(true);

            }

// called by UsbCentral::service, then pass on to registered service
// (like cdcacm)
//=============================================================================
            auto UsbCentral::
service     (uint8_t ustat, UsbEP0* ep) -> bool
            {
            if(m_service) return m_service(ustat, ep);
            return false;
            }

// get desired descriptor from descriptor info
// pkt.wValue high=descriptor type, low=index
// device=1, config=2, string=3
//=============================================================================
            auto UsbCentral::
get_desc    (uint16_t wValue, uint16_t* siz) -> const uint8_t*
            {
            if(not m_descriptor){ *siz = 0; return 0; }
            uint8_t idx = wValue;
            uint8_t typ = wValue>>8;
            uint16_t i = 0;
            //find header type, if idx > 0, also find header[idx]
            for(;;){
                if(m_descriptor[i+1] == typ){
                    if(not idx) break;     //found it
                    idx--;                 //wrong index, dec until 0
                }
                i += m_descriptor[i];
                if(not i){ *siz = 0; return 0; } //at end (0 marker)
            }
            //we now have index into descriptor
            //get size of type, if config get total size
            uint16_t actsiz = typ == UsbCh9::CONFIGURATION ?
                m_descriptor[i+2] + (m_descriptor[i+3]<<8) ://total config size
                m_descriptor[i]; //size of others
            //limit to actual size
            if(*siz > actsiz) *siz = actsiz;
            return &m_descriptor[i];
            }

// set descriptor wanted (set before usb init), and service function
// this will set the interface from user created device like cdcacm to
// UsbCentral
//=============================================================================
            auto UsbCentral::
set_device  (const uint8_t* d, service_t f) -> bool
            {
            m_descriptor = d;
            m_service = f;
            return init(d and f);
            }

// return configuration descriptor pointer if found, 0 if not
// also used to find location of current configuration in descriptor
//=============================================================================
            auto UsbCentral::
set_config  (uint16_t wValue) -> const uint8_t*
            {
            wValue and_eq 0xFF;                         //low byte only
            uint16_t wv = UsbCh9::CONFIGURATION<<8;     //get first config (0)
            uint16_t siz;                               //needed, not used
            for(;; wv++){                               //until config# match
                const uint8_t* p = get_desc(wv, &siz);  //get pointer to config
                if(not p) break;                        //no more configs left
                if(p[5] == wValue){                     //matching number?
                    current_config = wValue;            //save it
                    return p;                           //pointer to config
                }
            }
            return 0;                                   //bad config#
            }

// get endpoint n tx or rx size from descriptor
// endpoint 0 size is in first descriptor (both tx,rx are same size)
//=============================================================================
            auto UsbCentral::
get_epsiz   (uint8_t n, bool tr) -> uint16_t
            {
            if(not m_descriptor) return 0;
            if(n == 0) return m_descriptor[7];              //ep0
            if(tr) n += 128;                                //if tx, set bit7
            const uint8_t* p = set_config(current_config);  //get config offset
            for(; p and p[1] != UsbCh9::STRING; p += p[0]){ //until into STRING
                if(p[1] != UsbCh9::ENDPOINT or p[2] != n) continue;
                return p[4] bitor (p[5]<<8);                //found,return size
            }
            return 0;                                       //not found
            }

// get endpoint n control info from descriptor
// endpoint 0 always tx/rx/handshake
// others get from descriptor info
//=============================================================================
            auto UsbCentral::
get_epctrl  (uint8_t n) -> uint8_t
            {
            if(not m_descriptor) return 0;
            if(n == 0) return Usb::EPTXEN|Usb::EPRXEN|Usb::EPHSHK; //ep0
            uint8_t ret = Usb::EPCONDIS; //assume no control pkts on all others
            const uint8_t* p = set_config(current_config);  //get config offset
            for(; p and p[1] != UsbCh9::STRING; p += p[0]){ //until into STRING
                if(p[1] != UsbCh9::ENDPOINT) continue;      //not an endpoint
                if(p[2] == n) ret or_eq Usb::EPRXEN;        //if rx, enable rx
                if(p[2] == n+128) ret or_eq Usb::EPTXEN;    //if tx, enbale tx
                //enable handshake unless isochronous
                if(p[3] != UsbCh9::ISOCHRONOUS) ret or_eq Usb::EPHSHK;
            }
            return ret;
            }

// get self-powered, remote wakeup info from descriptor
// return bits in postitions as needed by DEV_GET_STATUS
//=============================================================================
            auto UsbCentral::
get_status  () -> uint8_t
            {
            if(not m_descriptor) return 0;
            const uint8_t* p = set_config(current_config);  //get config offset
            if(not p) return 0;
            uint8_t ret = 0;
            if(p[7] bitand 0x40) ret = 1;                   //self-powered
            if(p[7] bitand 0x20) ret or_eq 2;               //remote wakeup
            return ret;
            }

// get 1ms timer count
//=============================================================================
            auto UsbCentral::
timer1ms    () -> uint32_t
            {
            return m_timer1ms;
            }

// get sof count
//=============================================================================
            auto UsbCentral::
sofcount    () -> uint32_t
            {
            return m_sofcount;
            }
