#pragma once

//USB Config settings

#include <cstdint>
#include "Pins.hpp"
#include "Usb.hpp"

struct UsbConfig {

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    /////// user provided data ////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    //---power/wakeup options-------------------------------------------------------
    static const bool self_powered = true;      //self-powered=1, bus-powered=0
    static const bool remote_wakeup = false;    //remote wakeup enabled=1

    //---interrupt------------------------------------------------------------------
    static const uint8_t usb_irq_pri = 1;       //usb interrupt priority
    static const uint8_t usb_irq_subpri = 0;    //usb interrupt sub-priority

    //---endpoints------------------------------------------------------------------
    static const uint8_t last_ep_num = 2;       //last endpoint number used

    static constexpr uint8_t ep_siz[(last_ep_num+1)<<1] = {
        64, 64,     //0 - out,in
        0, 8,       //1 - out,in
        64, 64      //2 - out,in
    };

    static constexpr uint8_t ep_ctrl[last_ep_num+1] = {
        Usb::EPTXEN|Usb::EPRXEN|Usb::EPHSHK,                //ep0
        Usb::EPTXEN|Usb::EPHSHK,                            //ep1
        Usb::EPTXEN|Usb::EPRXEN|Usb::EPCONDIS|Usb::EPHSHK   //ep2
    };

    //---vbus pin-------------------------------------------------------------------
    static const Pins::RPN vbus_pin_n = Pins::B6; //fixed pin, check datasheet

    //---debug----------------------------------------------------------------------
    static const bool debug_on = true;          //debug usb
};

    //also need outside the class to access
    constexpr uint8_t UsbConfig::ep_ctrl[];
    constexpr uint8_t UsbConfig::ep_siz[];

