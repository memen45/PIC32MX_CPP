#include "UsbCdcAcm.hpp"
#include "UsbCh9.hpp"
#include "UsbEP.hpp"
#include "UsbCentral.hpp"
#include "Usb.hpp"
#include <cstring> //strlen
#include <cstdio>  //debug printf


// sorry, a few defines here to make descriptor construction a little easier

// constexpr function to get first char of stringified macro arg
constexpr char char0(const char* str){
    return str[0] ? str[0] : ' '; // if empty string, return space (' ')
}

// word -> byte, byte - any word (2bytes) in descriptor, use W(word)
#define W(v) (uint8_t)(v), (v) >> 8

// to wide char (uses char0 function to get first char of stringified arg)
// _(Z) -> 'Z', 0  (so no single quotes needed)
#define _(x) char0(#x), 0

// for string descriptor type, use SD(_(s),_(t),_(r),_(i),_(n),_(g))
#define SD(...) (sizeof((char[]){__VA_ARGS__}))+2,      /*total length*/    \
                UsbCh9::STRING,                         /*type*/            \
                __VA_ARGS__                             /*wide char data*/

// for configuration descriptor type
#define CD(...) 9,                                      /*size of config*/  \
                UsbCh9::CONFIGURATION,                  /*type*/            \
                W((sizeof((uint8_t[]){__VA_ARGS__}))+4),/*total length*/    \
                __VA_ARGS__                             /*data*/


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DESCRIPTOR STARTS HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//CDC-ACM
static const uint8_t m_descriptor[] = {

    //==== device descriptor ====

    18,                 //length(always 18)
    UsbCh9::DEVICE,     //1
    W(0x0101),          //0x0200 //(bcd) usb 1.1 (prevent unneeded inquiries)
    2,                  //class (CDC device)
    0,                  //subclass
    0,                  //protocol
    64,                 //max ep0 packet size (IN/OUT same)
    W(0x04D8),          //VID
    W(0x000A),          //PID
    W(0x0100),          //(bcd) release number
    1,                  //string index- manufacturer
    2,                  // -product
    0,                  // -serial number
    1,                  //#of configurations


    //==== config 1 ====

    CD(
        //(9, UsbCh9::CONFIGURATION, total size -> done by macro)
        //configuration
        2,                      //number of interfaces
        1,                      //this configuration number
        0,                      //string index for this config
        UsbCh9::SELFPOWER,      //self-powered and remote-wakup
        50,                     //bus power required, in 2ma units

        //interface0
        9, UsbCh9::INTERFACE, 0, 0, 1, 2, 2, 1, 0,

        //cdc descriptors
        5, 36, 0, W(0x110),     //cdc header
        4, 36, 2, 2,            //acm, D0-D3, D1 set- line coding/state
        5, 36, 6, 0, 1,         //union comm id=0, data id=1
        5, 36, 1, 0, 1,         //call management

        //endpoint
        7, UsbCh9::ENDPOINT, UsbCh9::IN1, UsbCh9::INTERRUPT, W(8), 2,

        //interface1
        9, UsbCh9::INTERFACE, 1, 0, 2, 10, 0, 0, 0,

        //endpoint
        7, UsbCh9::ENDPOINT, UsbCh9::OUT2, UsbCh9::BULK, W(64), 0,

        //endpoint
        7, UsbCh9::ENDPOINT, UsbCh9::IN2, UsbCh9::BULK, W(64), 0

    ), //end CD (configuration descriptor)

    //==== strings ====

    SD(W(0x0409)),                              //language 0x0409

    SD(_(P),_(I),_(C),_(3),_(2),_(M),_(M)),     //manufacturer (idx 1)

    SD(_(C),_(D),_(C),_(-),_(A),_(C),_(M)),     //product (idx 2)

    0 //end of descriptor marker - NEED to know where descriptor ends
      //since we iterrate over descriptor in our desciptor functions
};




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DEVICE FUNCTIONS START HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------private-----
static UsbEP                m_ep_state;  //ep1 = serial state
static const uint8_t        m_ep_state_num = 1;
static UsbEP                m_ep_txrx;   //ep2 = tx/rx
static const uint8_t        m_ep_txrx_num = 2;
static bool                 m_is_active = false;

//line coding- just store what pc wants, also return if wanted
using line_coding_t = struct {
    uint32_t    baud;
    uint8_t     stop_bits;  //0-1,1=1.5,2=2
    uint8_t     parity;     //none=0,even, odd, mark, space
    uint8_t     data_bits;  //5,6,7,8,16
};
static line_coding_t        m_line_coding = {115200, 0, 0, 8};

//SetupPkt_t.bRequest
enum SETUP_BREQUEST_CDC {
    CDC_BMREQUEST_TYPE =            0x21, //bmRequestType
    CDC_SEND_ENCAP_COMMAND =        0x00,
    CDC_GET_ENCAP_RESPONSE =        0x01,
    CDC_SET_COMM_FEATURE =          0x02,
    CDC_GET_COMM_FEATURE =          0x03,
    CDC_CLEAR_COMM_FEATURE =        0x04,
    CDC_SET_LINE_CODING =           0x20,
    CDC_GET_LINE_CODING =           0x21,
    CDC_SET_CONTROL_LINE_STATE =    0x22,
    CDC_SEND_BREAK =                0x23
};

//-------------------------------------------------------------------debug-----
//debug line coding being set in ep0->recv, call here after data received
bool ep0callback(UsbEP* ep0){
    printf("UsbCdcAcm:: m_line_coding $3%d %d %d %d$7\r\n",
            m_line_coding.baud, m_line_coding.stop_bits,
            m_line_coding.parity, m_line_coding.data_bits);
    return true;
}

// any endpoint 0 setup requests for us handled here
// we take care of endpoint 0 rx/tx here (if for us), but endpoint 0 will
// take care of next setup rx
//-----------------------------------------------------------------private-----
            static auto
ep0_request (UsbEP0* ep0) -> bool
            {
            if(ep0->setup_pkt.bmRequestType != CDC_BMREQUEST_TYPE) return false;
            printf("UsbCdcAcm::ep0_request  $3%02x$7\r\n",
                ep0->setup_pkt.bRequest);
            switch(ep0->setup_pkt.bRequest){
                case CDC_SET_LINE_CODING:
                    //callback will print debug info after line coding data received
                    ep0->recv((uint8_t*)&m_line_coding, 7, true, ep0callback); //rx data,
                    ep0->send((uint8_t*)&m_line_coding, 0, true); //tx status
                    return true;
                case CDC_GET_LINE_CODING:
                    ep0->send((uint8_t*)&m_line_coding, 7, true); //tx data,
                    ep0->recv((uint8_t*)&m_line_coding, 0, true); //rx status
                    return true;
                case CDC_SET_CONTROL_LINE_STATE:
                    printf("UsbCdcAcm::ep0_request  set control line state: $4RTS: %d DTR: %d$7\r\n",
                        (ep0->setup_pkt.wValue bitand 2) >> 1, ep0->setup_pkt.wValue bitand 1);
                    ep0->send((uint8_t*)&m_line_coding, 0, true); //tx status
                    return true;
            }
            return false; //unhandled
            }

// UsbCentral will call us with usb stat value
// if 0xFF, is signalling a reset or detach
// else handle endpoint transactions, or if ep0 set check setup request
//-----------------------------------------------------------------private-----
            static auto
service     (uint8_t ustat, UsbEP0* ep0) -> bool
            {
            //check for reset (detach)
            if(ustat == 0xFF){                  //called by UsbDevice::attach
                m_ep_state.init(m_ep_state_num);//via UsbCentral::service
                m_ep_txrx.init(m_ep_txrx_num);  //init our endpoints
                m_is_active = false;            //not active
                return true;
            }
            //init sets active, but the reset above also happens in the
            //initial setup, so anytime we get here we must be active
            //so turn it back on
            m_is_active = true;

            //ustat = ep<5:2> dir<1> even/odd<0>
            uint8_t n = ustat>>2;

            //handle tx/rx
            if(n == m_ep_txrx_num) return m_ep_txrx.service(ustat);

            //handle serial state (unused for now)
            if(n == m_ep_state_num) return true;

            //not our endpoint, check if setup request on ep0
            //handle any endpoint 0 setup requests
            if(ep0) return ep0_request(ep0);

            //not for us
            return false;
            }

// called by user app to init or deinit (turn off, detach)
// returns result of usb init (is active = true)
//=============================================================================
            auto UsbCdcAcm::
init        (bool tf) -> bool
            {
            m_is_active = UsbCentral::set_device(m_descriptor,
                                                 tf ? service : 0);
            return m_is_active;
            }

// called by user app to send data, optionally set callback when send complete
// buf = buffer pointer, siz = bytes to send, optional notify func pointer
//=============================================================================
            auto UsbCdcAcm::
send        (uint8_t* buf, uint16_t siz, UsbEP::notify_t f) -> bool
            {
            return m_ep_txrx.send_busy() ? false : m_ep_txrx.send(buf, siz, f);
            }

// called by user app to send data, optionally set callback when send complete
// buf = buffer pointer, optional notify func pointer
// (this one will get the buffer size, so cannot have embedded 0's)
//=============================================================================
            auto UsbCdcAcm::
send        (const char* buf, UsbEP::notify_t f) -> bool
            {
            uint16_t siz = strlen( buf );
            return m_ep_txrx.send_busy() ?
                false : m_ep_txrx.send((uint8_t*)buf, siz, f);
            }

// called by user app to recv data, optionally set callback when recv complete
// buf = buffer pointer, siz = bytes to recv, optional notify func pointer
// if receive less than full packet, recv will be considered complete
//=============================================================================
            auto UsbCdcAcm::
recv        (uint8_t* buf, uint16_t siz, UsbEP::notify_t f) -> bool
            {
            return m_ep_txrx.recv_busy() ? false : m_ep_txrx.recv(buf, siz, f);
            }

// check if usb active (may have detached)
// user app can check, try to init again if no longer active
//=============================================================================
            auto UsbCdcAcm::
is_active   () -> bool
            {
            return m_is_active;
            }

// check if usb busy
//=============================================================================
            auto UsbCdcAcm::
busy        (TXRX e) -> bool
            {
            return m_is_active and
                e == SEND ? m_ep_txrx.send_busy() : m_ep_txrx.recv_busy() ;
            }
