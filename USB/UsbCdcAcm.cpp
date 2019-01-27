#include "UsbCdcAcm.hpp"
#include "UsbCh9.hpp"
#include "UsbEP.hpp"
#include "UsbCentral.hpp"
#include "Usb.hpp"
#include <cstring> //strlen
#include <cstdio>  //debug printf
#include "UsbDebug.hpp"

UsbCh9 ch9; //so we can shorten usage from :: to .

//(rare) defines here to make descriptor construction easier

// word -> byte, byte - any word (2bytes) in descriptor, use W(word)
#define W(v) (uint8_t)(v), (v) >> 8

// to wide char
// _(Z) -> 'Z', 0  (so no single quotes needed)
// will need to use _() for any space characters
#define _(x) ((char*)#x)[0] ? ((char*)#x)[0] : ' ', 0

// for descriptors
#define DESC(typ, ...) \
    (sizeof((uint8_t[]){__VA_ARGS__}))+2, /*size of descriptor*/ \
    (uint8_t)typ,                         /*type*/ \
    __VA_ARGS__                           /*data*/

// device descriptor
#define DEVd(...) DESC(ch9.DEVICE, __VA_ARGS__)
// for configuration descriptor type
#define CONFd(...) 9,                       /*size of config*/  \
    ch9.CONFIGURATION,                      /*type*/            \
    W((sizeof((uint8_t[]){__VA_ARGS__}))+4),/*total length*/    \
    __VA_ARGS__                             /*data*/

// for interface descriptor type
#define IFd(...) DESC(ch9.INTERFACE, __VA_ARGS__)
// for cdc descriptor type (communications class, CS_INTERFACE)
#define CDCd(...) DESC(0x24, __VA_ARGS__)
// for endpoint descriptor type
#define EPd(...) DESC(ch9.ENDPOINT, __VA_ARGS__)
// for string descriptor type, use SD(_(s),_(t),_(r),_(i),_(n),_(g))
#define STRd(...) DESC(ch9.STRING, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// USER CREATED DESCRIPTOR STARTS HERE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//CDC-ACM
static const uint8_t m_descriptor[] = {

    //==== device descriptor ====

    DEVd(
        W(0x0101),          //0x0200/0x0101 (usb 1.1 has less inquiries)
        2, 0, 0,            //class (CDC device), subclass, prorocol
        64,                 //max ep0 packet size (IN/OUT same)
        W(0x04D8),W(0x000A),//VID, PID
        W(0x0100),          //(bcd) release number
        1, 2, 3,            //string index- manufacturer, product, serial
        1                   //#of configurations
    ), //end device descriptor

    //==== config 1 ====

    CONFd(
        //configuration
        2,                      //number of interfaces
        1,                      //this configuration number
        0,                      //string index for this config
        ch9.SELFPOWER,          //self-powered and remote-wakup
        50,                     //bus power required, in 2ma units

        //interface0
        IFd( 0, 0, 1, 2, 2, 1, 0 ),

        //cdc descriptors
        CDCd( 0, W(0x110) ),    //cdc header
        CDCd( 2, 2 ),           //acm, D0-D3, D1 set- line coding/state
        CDCd( 6, 0, 1 ),        //union comm id=0, data id=1
        CDCd( 1, 0, 1 ),        //call management

        //endpoint
        EPd( ch9.IN1, ch9.INTERRUPT, W(8), 2 ),
        //interface1
        IFd( 1, 0, 2, 10, 0, 0, 0 ),
        //endpoint
        EPd( ch9.OUT2, ch9.BULK, W(64), 0 ),
        //endpoint
        EPd( ch9.IN2, ch9.BULK, W(64), 0 )

    ), //end configuration descriptor

    //==== strings ====

    STRd( W(0x0409) ),                            //language 0x0409
    STRd( _(P),_(I),_(C),_(3),_(2),_(M),_(M) ),   //manufacturer (idx 1)
    STRd( _(C),_(D),_(C),_(-),_(A),_(C),_(M) ),   //product (idx 2)
    STRd( _(1),_(2),_(3),_(4),_(5),_(6),_(7) ),   //serial# (idx 3)

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
//DEBUG
static const char*          m_filename = "UsbCdcAcm.cpp";

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
bool showlinecoding(UsbEP* ep0){

//     printf("UsbCdcAcm:: m_line_coding $3%d %d %d %d$7\r\n",
//             m_line_coding.baud, m_line_coding.stop_bits,
//             m_line_coding.parity, m_line_coding.data_bits);
//     return true;

    // * * * * DEBUG * * * *
    UsbDebug dbg;
    if( dbg.debug() ){
        dbg.debug( m_filename, __func__,
            "$3%d %d %d %d$7",
            m_line_coding.baud, m_line_coding.stop_bits,
            m_line_coding.parity, m_line_coding.data_bits );
    }
    // * * * * DEBUG * * * *
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

            // * * * * DEBUG * * * *
            UsbDebug dbg;
            if( dbg.debug() ){
                dbg.debug( m_filename, __func__,
                    "$3%02x$7",
                    ep0->setup_pkt.bRequest );
            }
            // * * * * DEBUG * * * *

            switch(ep0->setup_pkt.bRequest){
                case CDC_SET_LINE_CODING:
                    //callback will print debug info after line coding data received
                    ep0->recv((uint8_t*)&m_line_coding, 7, true, showlinecoding); //rx data,
                    ep0->send((uint8_t*)&m_line_coding, 0, true); //tx status
                    return true;
                case CDC_GET_LINE_CODING:
                    ep0->send((uint8_t*)&m_line_coding, 7, true); //tx data,
                    ep0->recv((uint8_t*)&m_line_coding, 0, true); //rx status
                    return true;
                case CDC_SET_CONTROL_LINE_STATE:

                    // * * * * DEBUG * * * *
                    UsbDebug dbg;
                    if( dbg.debug() ){
                        dbg.debug( m_filename, __func__,
                            "$4RTS: %d DTR: %d$7",
                            (ep0->setup_pkt.wValue bitand 2) >> 1,
                            ep0->setup_pkt.wValue bitand 1 );
                    }
                    // * * * * DEBUG * * * *

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
            return send( (uint8_t*)buf, siz, f );
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
